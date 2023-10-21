#include <csignal>
#include <fstream>
#include <filesystem>
#include <sys/stat.h>
#include "TcpServer.hpp"
#include "Configuration.hpp"
#include "Client.hpp"
#include "MultiplexerIO.hpp"
#include "HttpRequest.hpp"
#include "Timer.hpp"
#include "HttpResponse.hpp"

#define BUFSIZE 256
#define PARSTER false // change this

void handleConnectedClient(Client *client, std::vector<Socket *> &toBeDeleted)
{
    char buf[BUFSIZE + 1]{}; // Buffer for client data
    int nbytes{static_cast<int>(recv(client->m_socketFd, buf, BUFSIZE, 0))};
    if (nbytes <= 0)
    {
        close(client->m_timer->m_socketFd);
        client->m_timer->m_socketFd = -1;
        toBeDeleted.push_back(client->m_timer);

        close(client->m_socketFd);
        client->m_socketFd = -1;
        toBeDeleted.push_back(client);

        return;
    }

    try
    {
        if (client->m_request.tokenize(buf, nbytes))
            return;

        if (timerfd_settime(client->m_timer->m_socketFd, 0, &client->m_timer->m_spec, NULL) == -1)
            throw StatusCodeException(500, "Error: timerfd_settime()");

        client->m_request.parse();
    }
    catch (const StatusCodeException &e)
    {
        client->m_request.m_response.m_statusCode = e.getStatusCode();
        spdlog::warn(e.what());

        for (const auto &errorPageConfig : client->m_request.m_serverconfig.getErrorPagesConfig())
        {
            for (const auto &errorCode : errorPageConfig.getErrorCodes())
            {
                if (errorCode == client->m_request.m_response.m_statusCode)
                {
                    try // try catch in case error page doesnt exist. Is it possible to check all files during parsing?
                    {
                        client->m_request.m_response.m_body = Helper::fileToStr(errorPageConfig.getFilePath());
                    }
                    catch (const std::exception &e)
                    {
                        std::cerr << e.what() << '\n';
                    }
                }
            }
        }
    }
    spdlog::critical(client->m_request);

    close(client->m_timer->m_socketFd);
    client->m_timer->m_socketFd = -1;
    toBeDeleted.push_back(client->m_timer);

    MultiplexerIO &multiplexerio = MultiplexerIO::getInstance();
    multiplexerio.modifyEpollEvents(client, EPOLLOUT);
}

void run(const Configuration &config)
{
    std::vector<TcpServer *> servers{};

    for (auto &serverConfig : config.serversConfig)
        servers.push_back(new TcpServer{serverConfig});

    MultiplexerIO &multiplexerio = MultiplexerIO::getInstance();
    for (auto &server : servers)
        multiplexerio.addSocketToEpollFd(server, EPOLLIN | EPOLLRDHUP);

    while (true)
    {
        std::vector<Socket *> toBeDeleted{};

        for (auto &s : toBeDeleted)
            delete s;

        int epollCount = epoll_wait(multiplexerio.m_epollfd, multiplexerio.m_events.data(), MAX_EVENTS, -1);
        if (epollCount < 0)
        {
            std::perror("epoll_wait() failed");
            throw std::runtime_error("Error: epoll_wait() failed\n");
        }

        for (int i = 0; i < epollCount; i++) // Run through the existing connections looking for data to read
        {
            Socket *ePollDataPtr = static_cast<Socket *>(multiplexerio.m_events[i].data.ptr);

            if (ePollDataPtr->m_socketFd == -1)
                continue;

            if (multiplexerio.m_events[i].events & EPOLLIN) // If someone's ready to read
            {
                if (TcpServer *server = dynamic_cast<TcpServer *>(ePollDataPtr)) // If listener is ready to read, handle new connection
                {
                    Client *client = new Client{*server};
                    multiplexerio.addSocketToEpollFd(client, EPOLLIN | EPOLLRDHUP);
                    multiplexerio.addSocketToEpollFd(client->m_timer, EPOLLIN | EPOLLRDHUP);
                }
                else if (Client *client = dynamic_cast<Client *>(ePollDataPtr)) // If not the listener, we're just a regular client
                    handleConnectedClient(client, toBeDeleted);
                else if (Timer *m_timer = dynamic_cast<Timer *>(ePollDataPtr))
                {
                    spdlog::warn("Timeout expired. Closing: {}", *(m_timer->m_client));

                    close(m_timer->m_client->m_socketFd);
                    m_timer->m_client->m_socketFd = -1;
                    toBeDeleted.push_back(m_timer->m_client);

                    close(m_timer->m_socketFd);
                    m_timer->m_socketFd = -1;
                    toBeDeleted.push_back(m_timer);
                }
            }
            else if (multiplexerio.m_events[i].events & EPOLLOUT) // If someone's ready to write
            {
                if (Client *client = dynamic_cast<Client *>(ePollDataPtr))
                {
                    if (client->m_request.m_response.m_buf.empty())
                    {
                        client->m_request.m_response.m_buf = client->m_request.m_response.responseBuild();
                        client->m_request.m_response.m_len = client->m_request.m_response.m_buf.size();
                        client->m_request.m_response.m_bytesleft = client->m_request.m_response.m_len;
                    }

                    if (client->m_request.m_response.m_total < client->m_request.m_response.m_len)
                    {
                        int nbytes{static_cast<int>(send(client->m_socketFd, client->m_request.m_response.m_buf.data() + client->m_request.m_response.m_total, client->m_request.m_response.m_bytesleft, 0))};
                        if (nbytes <= 0)
                        {
                            close(client->m_socketFd);
                            client->m_socketFd = -1;
                            toBeDeleted.push_back(client);

                            continue;
                        }
                        client->m_request.m_response.m_total += nbytes;
                        client->m_request.m_response.m_bytesleft -= nbytes;
                    }

                    if (!client->m_request.m_response.m_bytesleft)
                    {
                        close(client->m_socketFd);
                        client->m_socketFd = -1;
                        toBeDeleted.push_back(client);
                    }
                }
            }
            else
            {
                spdlog::critical("Unexpected");
            }
            if (multiplexerio.m_events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
            {
                close(ePollDataPtr->m_socketFd);
                ePollDataPtr->m_socketFd = -1;
                toBeDeleted.push_back(ePollDataPtr);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    spdlog::set_level(spdlog::level::debug);

    if (argc != 2)
        throw std::runtime_error("usage: webserv [port]\n\n\n");

    Configuration config{};
    if (initConfig(argv[1], config))
    {
        std::cout << "Config failure" << std::endl;
        return 1;
    }

#if (PARSTER) // To run only the parser and display the output
    std::cout << "\n\t\t -----------------\n \t\t|  SERVER CONFIG  |\n\t\t -----------------\n";
    for (size_t i = 0; i < config.serversConfig.size(); ++i)
    {
        std::cout << config.serversConfig[i];
    }
    return 0;
#endif

    run(config);

    return 0;
}