#include "TcpServer.hpp"
#include "ClientSocket.hpp"
#include "serverIO.hpp"
#include <sys/epoll.h>

int main(void)
{
    TcpServer t;
    ServerIO s;
    int nReadyFds{};
    char buffer[BUFSIZE]{};
    ssize_t nBytesReceived;
    int currEventFd;

    s.addSocketToEpollFd(t.m_serverSocket);

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
            if (s.m_events.at(i).data.fd == t.m_serverSocket)
            {
                ClientSocket c;

                c.m_clientSocket = accept(t.m_serverSocket, (struct sockaddr *)&c.m_clientAddr, &c.m_clientAddrSize);
                if (c.m_clientSocket == -1)
                {
                    std::perror("accept() failed");
                    throw std::runtime_error("Error: accept() failed\n");
                }
                t.m_clientSockets.push_back(c);
                s.addSocketToEpollFd(c.m_clientSocket);
            }
            else
            {
                currEventFd = s.m_events.at(i).data.fd;

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
