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

void handleConnectedClient(Client *client)
{
    char buf[BUFSIZE + 1]{}; // Buffer for client data
    int nbytes = recv(client->m_socketFd, buf, BUFSIZE, 0);
    if (nbytes <= 0)
    {
        if (nbytes == 0) // Connection closed
            spdlog::info("socket {} hung up", *client);
        else if (nbytes < 0) // Got error or connection closed by client
            spdlog::critical("Error: recv() failed");
        delete client;
        return;
    }

    try
    {
        if (client->m_request.tokenize(buf, nbytes))
            return;

        if (timerfd_settime(client->timer->m_socketFd, 0, &client->timer->m_spec, NULL) == -1)
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
                if (std::atoi(errorCode.c_str()) == client->m_request.m_response.m_statusCode)
                    client->m_request.m_response.m_body = Helper::fileToStr(errorPageConfig.getUriPath());
            }
        }
    }
    spdlog::critical(client->m_request);
    client->isWriteReady = true;
}

void run(const Configuration &config)
{
    std::vector<TcpServer *> servers{};
    for (auto &serverConfig : config.serversConfig)
        servers.push_back(new TcpServer{serverConfig});

    MultiplexerIO &multiplexerio = MultiplexerIO::getInstance();
    for (auto &server : servers)
        multiplexerio.addSocketToEpollFd(server, EPOLLIN);

    while (true)
    {
        int epollCount{epoll_wait(multiplexerio.m_epollfd, multiplexerio.m_events.data(), MAX_EVENTS, -1)};
        if (epollCount < 0)
        {
            std::perror("epoll_wait() failed");
            throw std::runtime_error("Error: epoll_wait() failed\n");
        }

        for (int i{0}; i < epollCount; i++) // Run through the existing connections looking for data to read
        {
            Socket *ePollDataPtr{static_cast<Socket *>(multiplexerio.m_events.at(i).data.ptr)};

            if ((multiplexerio.m_events.at(i).events & EPOLLIN) && ePollDataPtr->isReadReady) // If someone's ready to read
            {
                if (TcpServer *server = dynamic_cast<TcpServer *>(ePollDataPtr)) // If listener is ready to read, handle new connection
                {
                    Client *client = new Client{*server};
                    multiplexerio.addSocketToEpollFd(client, EPOLLIN | EPOLLOUT | EPOLLRDHUP);
                    multiplexerio.addSocketToEpollFd(client->timer, EPOLLIN);
                }
                else if (Timer *timer = dynamic_cast<Timer *>(ePollDataPtr))
                {
                    spdlog::warn("Timeout expired. Closing: {}", *(timer->m_client));
                    delete timer->m_client;
                }
                else if (Client *client = dynamic_cast<Client *>(ePollDataPtr)) // If not the listener, we're just a regular client
                    handleConnectedClient(client);
            }
            if ((multiplexerio.m_events.at(i).events & EPOLLOUT) && ePollDataPtr->isWriteReady) // If someone's ready to write
            {
                if (Client *client = dynamic_cast<Client *>(ePollDataPtr))
                {
                    std::string s{client->m_request.m_response.responseBuild()};
                    send(client->m_socketFd, s.data(), s.length(), 0);
                    delete (client);
                }
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