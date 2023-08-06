#include "TcpServer.hpp"
#include "ClientSocket.hpp"
#include "serverIO.hpp"
#include <sys/epoll.h>
#include <csignal>

volatile std::sig_atomic_t caughtSigint{false};

void handleSigint(int)
{
    caughtSigint = true;
}

int main(void)
{
    ServerIO s{};
    int nReadyFds{};
    char buffer[BUFSIZE]{};
    ssize_t nBytesReceived{};
    int currEventFd{};
    unsigned int args[2]{12345, 23456};
    // Used to simulate a Python for-else loop
    bool flag{false};

    std::signal(SIGINT, handleSigint);

    // Create TcpServer instance(s) for each port
    for (auto &arg : args)
    {
        std::unique_ptr<TcpServer> t = std::make_unique<TcpServer>(arg);
        s.m_servers.push_back(std::move(t));
    }

    // Loop over TcpServer instance(s) and add to EpollFd
    for (auto &server : s.m_servers)
        s.addSocketToEpollFd(server->m_serverSocket);

    while (true)
    {
        nReadyFds = epoll_wait(s.m_epollFD, s.m_events.data(), MAX_EVENTS, -1);
        if (nReadyFds == -1)
        {
            if (errno == EINTR && caughtSigint)
            {
                std::cout << "Caught SIGINT, shutting down gracefully...\n";
                break;
            }
            else
            {
                std::perror("epoll_wait() failed");
                throw std::runtime_error("Error: epoll_wait() failed\n");
            }
        }

        for (int i{0}; i < nReadyFds; i++)
        {
            currEventFd = s.m_events.at(i).data.fd;
            flag = false;
            for (auto &server : s.m_servers)
            {
                // Loop over TcpServer instance(s)
                if (currEventFd == server->m_serverSocket)
                {
                    // This is an event from a TcpServer instance, aka a new client requesting connection to this instance
                    std::unique_ptr<ClientSocket> c = std::make_unique<ClientSocket>();
                    c->m_clientSocket = accept(server->m_serverSocket, (struct sockaddr *)&(c->m_clientAddr), &(c->m_clientAddrSize));
                    if (c->m_clientSocket == -1)
                    {
                        std::perror("accept() failed");
                        throw std::runtime_error("Error: accept() failed\n");
                    }
                    s.addSocketToEpollFd(c->m_clientSocket);
                    server->m_clientSockets.push_back(std::move(c));

                    flag = true;
                    break;
                }
            }

            if (!flag)
            // Loop over TcpServer instance(s) was completed without finding a match
            // This means the event came from an already connected client socket
            {
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
