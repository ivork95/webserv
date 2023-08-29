#include <csignal>
#include "TcpServer.hpp"
#include "Client.hpp"
#include "MultiplexerIO.hpp"
#include "HttpMessage.hpp"
#include "HttpRequest.hpp"

#define BUFSIZE 256

void do_use_fd(Socket *ePollDataPtr)
{
    Client *c{};

    if ((c = dynamic_cast<Client *>(ePollDataPtr)))
        ;
    else
    {
        std::cerr << "Error: the actual type of the object pointed to by ePollDataPtr = Unrecognized\n";
        return;
    }

    char buf[BUFSIZE + 1]{}; // Buffer for client data
    int nbytes = recv(ePollDataPtr->m_socketFd, buf, BUFSIZE, 0);

    if (nbytes <= 0) // Got error or connection closed by client
    {
        if (nbytes == 0) // Connection closed
            std::cout << "socket " << *c << " hung up\n";
        else
            std::cerr << "Error: recv() failed\n";
        close(ePollDataPtr->m_socketFd);
    }
    else // We got some good data from a client
    {
        std::cout << "nbytes = " << nbytes << '\n';
        HttpMessage message(buf);
        std::cout << "\n\nBUF:\n"
                  << buf << "\n###\n\n";
        c->requestMessage += message;
        if (!c->requestMessage.isComplete())
        {
            std::cout << "Message not complete...\n\n";
            return;
        }
        std::cout << "Mission completed!!!\n\n"
                  << std::endl;
        std::cout << "\n\n|" << c->requestMessage.getRawRequest() << "|\n\n";
        if (c->requestMessage.isValidHttpMessage())
        {
            HttpRequest httpRequest{c->requestMessage};
            std::cout << "HttpMessage is valid\n\n";
            std::cout << httpRequest << std::endl;
            return;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        throw std::runtime_error("usage: webserv [port]\n\n\n");

    MultiplexerIO serverio{};

    for (int i{1}; i < argc; i++)
    {
        TcpServer *tcpserver = new TcpServer{argv[i]};
        serverio.m_servers.push_back(tcpserver);
    }

    for (auto &server : serverio.m_servers)
        serverio.addSocketToEpollFd(server);

    while (true)
    {
        int epollCount{epoll_wait(serverio.m_epollfd, serverio.m_events.data(), MAX_EVENTS, (3 * 60 * 1000))};
        if (epollCount == -1)
        {
            std::perror("epoll_wait() failed");
            throw std::runtime_error("Error: epoll_wait() failed\n");
        }

        for (int i{0}; i < epollCount; i++) // Run through the existing connections looking for data to read
        {
            bool isLoopBroken{false};
            Socket *ePollDataPtr{static_cast<Socket *>(serverio.m_events.at(i).data.ptr)};

            // Check if someone's ready to read
            if (serverio.m_events.at(i).events & EPOLLIN) // We got one!!
            {
                for (auto &server : serverio.m_servers)
                {
                    if (ePollDataPtr->m_socketFd == server->m_socketFd) // If listener is ready to read, handle new connection
                    {
                        Client *clientsocket = new Client{server->m_socketFd};
                        serverio.addSocketToEpollFd(clientsocket);
                        server->m_clientSockets.push_back(clientsocket);
                        isLoopBroken = true;
                        break;
                    }
                }
                if (!isLoopBroken) // If not the listener, we're just a regular client
                    do_use_fd(ePollDataPtr);
            }
        }
    }
}