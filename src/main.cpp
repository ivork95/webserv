#include <csignal>
#include <fstream>
#include <filesystem>
#include <sys/stat.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include "Server.hpp"
#include "Configuration.hpp"
#include "Client.hpp"
#include "Multiplexer.hpp"
#include "Request.hpp"
#include "Timer.hpp"
#include "Response.hpp"
#include "CGIPipeIn.hpp"
#include "CGIPipeOut.hpp"
#include "Signal.hpp"

#define PARSTER false

void handleRead(ASocket *&ePollDataPtr)
{
    Multiplexer &multiplexer = Multiplexer::getInstance();

    if (Client *client = dynamic_cast<Client *>(ePollDataPtr)) // Client is ready to read, handle incoming data
        client->handleConnectedClient();
    else if (Server *server = dynamic_cast<Server *>(ePollDataPtr)) // Server is ready to read, handle new connection
        server->handleNewConnection();
    else if (CGIPipeOut *pipeout = dynamic_cast<CGIPipeOut *>(ePollDataPtr))
        pipeout->readFromPipe();
    else if (Timer *timer = dynamic_cast<Timer *>(ePollDataPtr))
    {
        spdlog::info("Client timed out: {}", timer->m_client);
        multiplexer.removeFromEpoll(timer->m_socketFd);
        multiplexer.removeFromEpoll(timer->m_client.m_socketFd);
        multiplexer.removeClientBySocketFd(&timer->m_client);
    }
    else if (Signal *signal = dynamic_cast<Signal *>(ePollDataPtr))
        signal->readAndDelete();
}

void handleWrite(ASocket *&ePollDataPtr)
{
    Multiplexer &multiplexer = Multiplexer::getInstance();

    if (Client *client = dynamic_cast<Client *>(ePollDataPtr))
    {
        spdlog::info("Generating response for {}", *client);
        if (client->getRequest().getResponse().sendAll(client->m_socketFd, client->getServer().getServerConfig().getErrorPagesConfig()) <= 0)
        {
            spdlog::info("Response send {}:\n{}\n", *client, client->getRequest().getResponse().m_statusLine);
            if (multiplexer.modifyEpollEvents(nullptr, 0, client->m_socketFd) == -1)
                spdlog::error("modifyEpollEvents()");
            if (epoll_ctl(multiplexer.m_epollfd, EPOLL_CTL_DEL, client->m_socketFd, NULL) == -1)
                spdlog::error("epoll_ctl()");
            multiplexer.removeClientBySocketFd(client);
        }
    }
    else if (CGIPipeIn *pipein = dynamic_cast<CGIPipeIn *>(ePollDataPtr))
    {
        multiplexer.removeFromEpoll(pipein->m_pipeFd[WRITE]);
        pipein->dupCloseWrite();
        if (multiplexer.addToEpoll(&(pipein->m_client.getRequest().getPipeOut()), EPOLLIN, pipein->m_client.getRequest().getPipeOut().m_pipeFd[READ]) == -1)
            throw StatusCodeException(500, "addToEpoll()", errno);
        pipein->m_client.getRequest().getPipeOut().forkCloseDupExec();
    }
}

void run(const Configuration &config)
{
    Multiplexer &multiplexer = Multiplexer::getInstance();
    int epollCount{};
    int i{};
    ASocket *ePollDataPtr{};
    uint32_t events{};

    spdlog::info("Webserver running");
    try
    {
        for (auto &serverConfig : config.serversConfig)
            multiplexer.m_servers.push_back(new Server{serverConfig});
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        for (auto &server : multiplexer.m_servers)
            delete server;
        return;
    }

    while (multiplexer.isRunning)
    {
        epollCount = epoll_wait(multiplexer.m_epollfd, multiplexer.m_events.data(), MAX_EVENTS, -1);
        if (epollCount == -1)
        {
            std::cerr << "Error: epoll_wait()\n";
            for (auto &server : multiplexer.m_servers)
                delete server;
            for (auto &client : multiplexer.m_clients)
                delete client;
            return;
        }

        for (i = 0; i < epollCount; i++) // Loop through ready list
        {
            ePollDataPtr = static_cast<ASocket *>(multiplexer.m_events[i].data.ptr);
            if (ePollDataPtr == nullptr)
                continue;
            events = multiplexer.m_events[i].events;
            try
            {
                if (events & EPOLLIN)
                    handleRead(ePollDataPtr);
                else if (events & EPOLLOUT)
                    handleWrite(ePollDataPtr);
                else if (events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
                {
                    multiplexer.removeFromEpoll(ePollDataPtr->m_socketFd);
                    throw StatusCodeException(500, "EPOLLHUP", errno);
                }
            }
            catch (const StatusCodeException &e)
            {
                spdlog::critical("CAUGHT ERROR");
                std::cerr << e.what() << '\n';
                if (Client *client = dynamic_cast<Client *>(ePollDataPtr))
                {
                    client->getRequest().getResponse().statusCodeSet(e.getStatusCode());
                    multiplexer.modifyEpollEvents(client, EPOLLOUT, client->m_socketFd);
                }
                else if (CGIPipeOut *pipeout = dynamic_cast<CGIPipeOut *>(ePollDataPtr))
                {
                    pipeout->m_client.getRequest().getResponse().statusCodeSet(e.getStatusCode());
                    multiplexer.modifyEpollEvents(&pipeout->m_client, EPOLLOUT, pipeout->m_client.m_socketFd);
                }
                else if (CGIPipeIn *pipein = dynamic_cast<CGIPipeIn *>(ePollDataPtr))
                {
                    pipein->m_client.getRequest().getResponse().statusCodeSet(e.getStatusCode());
                    multiplexer.modifyEpollEvents(&pipein->m_client, EPOLLOUT, pipein->m_client.m_socketFd);
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    std::string inputFile{};
    std::ifstream configFile{};
    Configuration config{};

    if (argc == 1)
        inputFile = "config-files/default.conf";
    else if (argc == 2)
        inputFile = argv[1];
    else
    {
        std::cerr << "usage: webserv [path to config]\n\n\n";
        return 1;
    }

    if (!ParserHelper::isValidConfigExtension(inputFile))
        return 1;

    if (ParserHelper::openFile(&configFile, inputFile))
        return 1;

    if (config.initConfig(configFile))
    {
        if (!config.serverSections.empty())
            config.serverSections.clear();
        if (!config.serversConfig.empty())
            config.serversConfig.clear();
        std::cerr << "Configuration file failure\n";
        return 1;
    }

#if (PARSTER) // To run only the parser and display the output
	config.printConfig();
    return 0;
#endif

    spdlog::set_level(spdlog::level::info);
    run(config);

    return 0;
}