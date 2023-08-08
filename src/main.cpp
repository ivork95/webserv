#include <csignal>
#include "TcpServer.hpp"
#include "ClientSocket.hpp"
#include "ServerIO.hpp"

volatile std::sig_atomic_t caughtSigint{false};

void handleSigint(int)
{
    caughtSigint = true;
}

int main(void)
{
    ServerIO serverio{};
    int nReadyFds{};
    char buffer[BUFSIZE]{};
    ssize_t nBytesReceived{};
    int currEventFd{};
    unsigned int args[2]{12345, 23456};
    bool isLoopBroken{false};

    std::signal(SIGINT, handleSigint);

    for (auto &arg : args)
    {
        std::unique_ptr<TcpServer> tcpserver = std::make_unique<TcpServer>(arg);
        serverio.m_servers.push_back(std::move(tcpserver));
    }

    for (auto &server : serverio.m_servers)
        serverio.addSocketToEpollFd(server->m_serverSocket);

    while (true)
    {
        nReadyFds = epoll_wait(serverio.m_epollFD, serverio.m_events.data(), MAX_EVENTS, (3 * 60 * 1000));
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
            isLoopBroken = false;
            currEventFd = serverio.m_events.at(i).data.fd;
            for (auto &server : serverio.m_servers)
            {
                if (currEventFd == server->m_serverSocket)
                {
                    std::unique_ptr<ClientSocket> clientsocket = std::make_unique<ClientSocket>(server->m_serverSocket);

                    serverio.addSocketToEpollFd(clientsocket->m_clientSocket);
                    server->m_clientSockets.push_back(std::move(clientsocket));

                    isLoopBroken = true;
                    break;
                }
            }

            if (!isLoopBroken)
            {
                nBytesReceived = recv(currEventFd, buffer, BUFSIZE, 0);
                if (nBytesReceived < 0)
                    std::cerr << "Error: recv() failed\n";
                else if (nBytesReceived == 0)
                {
                    std::cout << "Client disconnected before sending data.\n";
                    serverio.deleteSocketFromEpollFd(currEventFd);
                }
                else
                {
                    if (nBytesReceived < BUFSIZE)
                        buffer[nBytesReceived] = '\0';
                    else
                        buffer[BUFSIZE - 1] = '\0';

                    std::cout << "Received from client: " << buffer << "\n";

                    if (send(currEventFd, buffer, nBytesReceived, 0) != nBytesReceived)
                        std::cerr << "Error: send() failed\n";
                }
            }
        }
    }

    return 0;
}
