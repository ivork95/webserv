#include "TcpServer.hpp"
#include "Client.hpp"
#include "serverIO.hpp"
#include <sys/epoll.h>

int main(void)
{
    TcpServer t;
    Client c;
    ServerIO s;
    int numEvents{};
    char buffer[BUFSIZE]{};
    ssize_t numBytes;

    s.addSocketToEpollFd(t.m_serverSocket);

    while (true)
    {
        std::cerr << "YOOO 0\n";
        // numEvents = epoll_wait(s.m_fd, s.m_events.data(), MAX_EVENTS, -1);
        numEvents = epoll_wait(s.m_fd, s.m_events2, MAX_EVENTS, -1);
        if (numEvents == -1)
        {
            std::perror("epoll_wait() failed");
            throw std::runtime_error("Error: epoll_wait() failed\n");
        }

        std::cerr << "YOOO 1\n";

        for (int i = 0; i < numEvents; i++)
        {
            // if (s.m_events[i].data.fd == t.m_serverSocket)
            if (s.m_events2[i].data.fd == t.m_serverSocket)
            {
                c.m_clientSocket = accept(t.m_serverSocket, (struct sockaddr *)&c.m_clientAddr, &c.m_clientAddrSize);
                if (c.m_clientSocket == -1)
                {
                    std::perror("accept() failed");
                    throw std::runtime_error("Error: accept() failed\n");
                }

                s.addSocketToEpollFd(c.m_clientSocket);
            }
            else
            {
                numBytes = recv(c.m_clientSocket, buffer, BUFSIZE, 0);
                if (numBytes < 0)
                {
                    std::perror("recv() failed");
                }
                else if (numBytes == 0)
                {
                    std::cout << "Client disconnected before sending data.\n";
                }
                else
                {
                    // Null-terminate the received data in case it's a string
                    if (numBytes < BUFSIZE)
                        buffer[numBytes] = '\0';
                    else
                        buffer[BUFSIZE - 1] = '\0';

                    // Print the received data
                    std::cout << "Received from client: " << buffer << "\n";

                    // Echo the data back to the client
                    if (send(c.m_clientSocket, buffer, numBytes, 0) != numBytes)
                        std::perror("send() failed to send all data");
                }
            }
        }
    }

    return 0;
}

/*
int main(void)
{
    ServerIO s;
    TcpServer t;
    Client c;
    char buffer[BUFSIZE];
    ssize_t numBytes;

    c.m_clientSocket = accept(t.m_serverSocket, (struct sockaddr *)&c.m_clientAddr, &c.m_clientAddrSize);
    while (true)
    {
        if (c.m_clientSocket == -1)
        {
            std::perror("accept() failed");
            throw std::runtime_error("Error: accept() failed\n");
        }

        numBytes = recv(c.m_clientSocket, buffer, BUFSIZE, 0);
        if (numBytes < 0)
        {
            std::perror("recv() failed");
        }
        else if (numBytes == 0)
        {
            std::cout << "Client disconnected before sending data.\n";
        }
        else
        {
            // Null-terminate the received data in case it's a string
            if (numBytes < BUFSIZE)
                buffer[numBytes] = '\0';
            else
                buffer[BUFSIZE - 1] = '\0';

            // Print the received data
            std::cout << "Received from client: " << buffer << "\n";

            // Echo the data back to the client
            if (send(c.m_clientSocket, buffer, numBytes, 0) != numBytes)
                std::perror("send() failed to send all data");
        }
    }
}
*/