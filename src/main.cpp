#include <csignal>
#include "TcpServer.hpp"
#include "Client.hpp"
#include "MultiplexerIO.hpp"
#include "HttpRequest.hpp"
#include <fstream>
#include "Timer.hpp"

#define BUFSIZE 256

void handleConnectedClient(Client *client)
{
    char buf[BUFSIZE + 1]{}; // Buffer for client data

    int nbytes = recv(client->m_socketFd, buf, BUFSIZE, 0);
    if (timerfd_settime(client->timer->m_socketFd, 0, &client->timer->m_spec, NULL) == -1)
    {
        perror("timerfd_settime");
        exit(EXIT_FAILURE);
    }

    if (nbytes <= 0) // Got error or connection closed by client
    {
        if (nbytes == 0) // Connection closed
            std::cout << "socket " << *client << " hung up\n";
        else
            std::cerr << "Error: recv() failed\n";
    }
    else // We got some good data from a client
    {
        spdlog::info("nbytes = {}", nbytes);
        spdlog::info("buf = \n|{}|", buf);

        client->httpRequest.m_rawMessage.append(buf);
        spdlog::info("client->httpRequest.m_rawMessage = \n|{}|", client->httpRequest.m_rawMessage);

        client->httpRequest.m_fieldLinesEndPos = client->httpRequest.m_rawMessage.find("\r\n\r\n");
        if (client->httpRequest.m_fieldLinesEndPos == std::string::npos)
        {
            spdlog::warn("message incomplete [...]");
            return;
        }

        if (client->httpRequest.m_method.empty())
        {
            client->httpRequest.setMethodPathVersion();
            client->httpRequest.setHeaders();
        }

        if (!client->httpRequest.m_method.compare("POST"))
        {
            if (!client->httpRequest.postRequestHandle())
                return;
        }
        else
        {
            spdlog::info("GET or OPTIONS method");
        }
    }
    delete client;
}

void run(int argc, char *argv[])
{
    std::vector<TcpServer *> servers{};
    for (int i{1}; i < argc; i++)
        servers.push_back(new TcpServer{argv[i]});

    MultiplexerIO multiplexerio{};
    for (auto &server : servers)
        multiplexerio.addSocketToEpollFd(server);

    while (true)
    {
        int epollCount{epoll_wait(multiplexerio.m_epollfd, multiplexerio.m_events.data(), MAX_EVENTS, (60 * 1000 * 3))};
        if (epollCount <= 0)
        {
            if (epollCount == 0)
            {
                std::perror("no file descriptor became ready during the requested timeout");
                throw std::runtime_error("Error: no file descriptor became ready during the requested timeout\n");
            }
            else
            {
                std::perror("epoll_wait() failed");
                throw std::runtime_error("Error: epoll_wait() failed\n");
            }
        }

        for (int i{0}; i < epollCount; i++) // Run through the existing connections looking for data to read
        {
            Socket *ePollDataPtr{static_cast<Socket *>(multiplexerio.m_events.at(i).data.ptr)};

            // Check if someone's ready to read
            if (multiplexerio.m_events.at(i).events & EPOLLIN) // We got one!!
            {
                if (TcpServer *server = dynamic_cast<TcpServer *>(ePollDataPtr))
                // If listener is ready to read, handle new connection
                {
                    Client *client = new Client{*server};
                    multiplexerio.addSocketToEpollFd(client);
                    // Timer *timer = new Timer;
                    multiplexerio.addSocketToEpollFd(client->timer);
                }
                else if (Timer *timer = dynamic_cast<Timer *>(ePollDataPtr))
                {
                    uint64_t exp{};

                    // Read the timer value
                    read(ePollDataPtr->m_socketFd, &exp, sizeof(uint64_t));
                    spdlog::warn("Timer expired, count: {}", (unsigned long long)exp);
                    delete timer->m_client;
                }
                else // If not the listener, we're just a regular client
                    handleConnectedClient(dynamic_cast<Client *>(ePollDataPtr));
            }
        }
    }
}

int main(int argc, char *argv[])
{
    /* Catch-all handler
        try
        {
            runGame();
        }
        catch(...)
        {
            std::cerr << "Abnormal termination\n";
        }
    */
    if (argc < 2)
        throw std::runtime_error("usage: webserv [port]\n\n\n");

    run(argc, argv);

    return 0;
}
