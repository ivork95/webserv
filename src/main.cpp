#include "TcpServer.hpp"
#include "ClientSocket.hpp"
#include "serverIO.hpp"
#include <sys/epoll.h>

int main(void)
{
    ServerIO s;
    int nReadyFds{};
    char buffer[BUFSIZE]{};
    ssize_t nBytesReceived;
    int currEventFd;
    unsigned int args[2]{12345, 23456};

    for (auto &arg : args)
    {
        TcpServer t{arg};
        s.m_servers.push_back(t);
    }

    for (auto &server : s.m_servers)
        s.addSocketToEpollFd(server.m_serverSocket);

    while (true)
    {
        nReadyFds = epoll_wait(s.m_epollFD, s.m_events.data(), MAX_EVENTS, TIMEOUT);
        if (nReadyFds == -1)
        {
            std::perror("epoll_wait() failed");
            throw std::runtime_error("Error: epoll_wait() failed\n");
        }

        for (int i = 0; i < nReadyFds; i++)
        {
            currEventFd = s.m_events.at(i).data.fd;
            // If the current fd is a server socket, accept the new connection
            for (auto &server : s.m_servers)
            {
                if (currEventFd == server.m_serverSocket)
                {
                    ClientSocket c;
                    c.m_clientSocket = accept(server.m_serverSocket, (struct sockaddr *)&c.m_clientAddr, &c.m_clientAddrSize);
                    if (c.m_clientSocket == -1)
                    {
                        std::perror("accept() failed");
                        throw std::runtime_error("Error: accept() failed\n");
                    }
                    server.m_clientSockets.push_back(c);
                    s.addSocketToEpollFd(c.m_clientSocket);
                    continue;
                }

                // If we've got here, this means that the current fd is not a server socket, so we assume it's a client
                nBytesReceived = recv(currEventFd, buffer, BUFSIZE, 0);
                if (nBytesReceived < 0)
                    std::cerr << "Error: recv() failed\n";
                else if (nBytesReceived == 0)
                {
                    std::cout << "Client disconnected before sending data.\n";
                    s.deleteSocketFromEpollFd(currEventFd);
                }
                else
                {
                    // Null-terminate the received data in case it's a string
                    if (nBytesReceived < BUFSIZE)
                        buffer[nBytesReceived] = '\0';
                    else
                        buffer[BUFSIZE - 1] = '\0';

                    // Print the received data
                    std::cout << "Received from client: " << buffer << "\n";

                    // Echo the data back to the client
                    if (send(currEventFd, buffer, nBytesReceived, 0) != nBytesReceived)
                        std::cerr << "Error: send() failed\n";
                }
            }
        }
    }

    return 0;
}
