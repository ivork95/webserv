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
    int bytesCount{};
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
            currEventFd = serverio.m_events.at(i).data.fd;
            uint32_t event = serverio.m_events.at(i).events;
            isLoopBroken = false;

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

            if (event & EPOLLIN)
            {
                if (!isLoopBroken)
                {
                    nBytesReceived = recv(currEventFd, buffer, BUFSIZE, 0);
                    bytesCount = bytesCount + nBytesReceived;
                    std::cout << "bytesCount = " << bytesCount << '\n';
                    if (nBytesReceived < 0)
                        std::cerr << "Error: recv() failed\n";
                    else if (nBytesReceived == 0)
                    {
                        std::cout << "Client disconnected before sending data.\n";
                        serverio.deleteSocketFromEpollFd(currEventFd);
                    }
                    else
                    {
                        std::cout << buffer;
                    }
                }
            }
        }
    }

    return 0;
}
