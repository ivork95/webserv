#include <csignal>
#include <fstream>
#include <filesystem>
#include <sys/stat.h>

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
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#define PARSTER false // change this

void handleHangUp(ASocket *&ePollDataPtr)
{
    Multiplexer &multiplexer = Multiplexer::getInstance();

    if (Client *client = dynamic_cast<Client *>(ePollDataPtr))
    {
        if (multiplexer.removeFromEpoll(client->m_socketFd) == -1)
            spdlog::error("removeFromEpoll()");
    }
    else if (CGIPipeOut *pipeout = dynamic_cast<CGIPipeOut *>(ePollDataPtr))
    {
        if (multiplexer.removeFromEpoll(pipeout->m_pipeFd[READ]) == -1)
            spdlog::error("removeFromEpoll()");
        if (multiplexer.modifyEpollEvents(&pipeout->m_client, EPOLLOUT, pipeout->m_client.m_socketFd) == -1)
            spdlog::error("modifyEpollEvents()");
        pipeout->m_client.m_request.m_response.m_statusCode = 500;
    }
    else if (CGIPipeIn *pipein = dynamic_cast<CGIPipeIn *>(ePollDataPtr))
    {
        if (multiplexer.removeFromEpoll(pipein->m_pipeFd[WRITE]) == -1)
            spdlog::error("removeFromEpoll()");
        if (multiplexer.modifyEpollEvents(&pipein->m_client, EPOLLOUT, pipein->m_client.m_socketFd) == -1)
            spdlog::error("modifyEpollEvents()");
        pipein->m_client.m_request.m_response.m_statusCode = 500;
    };
}

void handleRead(ASocket *&ePollDataPtr)
{
    Multiplexer &multiplexer = Multiplexer::getInstance();

    if (Client *client = dynamic_cast<Client *>(ePollDataPtr)) // Client is ready to read, handle incoming data
        client->handleConnectedClient();
    else if (Server *server = dynamic_cast<Server *>(ePollDataPtr)) // Server is ready to read, handle new connection
        server->handleNewConnection();
    else if (CGIPipeOut *pipeout = dynamic_cast<CGIPipeOut *>(ePollDataPtr))
    {
        if (epoll_ctl(multiplexer.m_epollfd, EPOLL_CTL_DEL, pipeout->m_pipeFd[READ], NULL) == -1)
            throw std::system_error(errno, std::generic_category(), "epoll_ctl()"); // status code err
        if (multiplexer.modifyEpollEvents(&pipeout->m_client, EPOLLOUT, pipeout->m_client.m_socketFd))
            throw std::system_error(errno, std::generic_category(), "modifyEpollEvents()");
        char buf[BUFSIZ]{};
        int nbytes{static_cast<int>(read(pipeout->m_pipeFd[READ], &buf, BUFSIZ - 1))};
        if (nbytes <= 0)
            pipeout->m_response.m_statusCode = 500;
        else
        {
            pipeout->m_response.m_body = buf;
            pipeout->m_response.m_statusCode = 200;
        }
    }
    else if (Timer *timer = dynamic_cast<Timer *>(ePollDataPtr))
    {
        multiplexer.modifyEpollEvents(nullptr, 0, timer->m_socketFd);
        epoll_ctl(multiplexer.m_epollfd, EPOLL_CTL_DEL, timer->m_socketFd, NULL);

        multiplexer.modifyEpollEvents(nullptr, 0, timer->m_client.m_socketFd);
        epoll_ctl(multiplexer.m_epollfd, EPOLL_CTL_DEL, timer->m_client.m_socketFd, NULL);

        multiplexer.removeClientBySocketFd(timer->m_client.m_socketFd);

        delete &timer->m_client;
    }
    else if (Signal *signal = dynamic_cast<Signal *>(ePollDataPtr))
        signal->readAndDelete();
}

void handleWrite(ASocket *&ePollDataPtr)
{
    Multiplexer &multiplexer = Multiplexer::getInstance();

    if (Client *client = dynamic_cast<Client *>(ePollDataPtr))
    {
        spdlog::debug("response = \n{}", client->m_request.m_response);
        if (client->m_request.m_response.sendAll(client->m_socketFd, client->m_server.m_serverconfig.getErrorPagesConfig()) <= 0)
        {
            multiplexer.modifyEpollEvents(nullptr, 0, client->m_socketFd);
            epoll_ctl(multiplexer.m_epollfd, EPOLL_CTL_DEL, client->m_socketFd, NULL);

            multiplexer.removeClientBySocketFd(client->m_socketFd);
            delete client;
        }
    }
    else if (CGIPipeIn *pipein = dynamic_cast<CGIPipeIn *>(ePollDataPtr))
    {
        spdlog::critical("Step 2\n");

        try
        {
            epoll_ctl(multiplexer.m_epollfd, EPOLL_CTL_DEL, pipein->m_pipeFd[WRITE], NULL);

            pipein->dupCloseWrite();

            if (multiplexer.addToEpoll(&(pipein->m_client.m_request.m_pipeout), EPOLLIN, pipein->m_client.m_request.m_pipeout.m_pipeFd[READ]) == -1)
                throw StatusCodeException(500, "addToEpoll()", errno);

            pipein->m_client.m_request.m_pipeout.forkCloseDupExec();
        }
        catch (const StatusCodeException &e)
        {
            std::cerr << e.what() << '\n';
            pipein->m_client.m_request.m_response.m_statusCode = e.getStatusCode();
            multiplexer.modifyEpollEvents(&(pipein->m_client), EPOLLOUT, pipein->m_client.m_socketFd);
        }
    }
}

void run(const Configuration &config)
{
    Multiplexer &multiplexer = Multiplexer::getInstance();
    int epollCount{};
    int i{};
    ASocket *ePollDataPtr{};
    uint32_t events{};

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
            throw std::system_error(errno, std::generic_category(), "epoll_wait()");

        for (i = 0; i < epollCount; i++) // Loop through ready list
        {
            ePollDataPtr = static_cast<ASocket *>(multiplexer.m_events[i].data.ptr);
            if (ePollDataPtr == nullptr)
                continue;
            events = multiplexer.m_events[i].events;
            try
            {
                switch (events)
                {
                case EPOLLIN:
                    handleRead(ePollDataPtr);
                    break;
                case EPOLLOUT:
                    handleWrite(ePollDataPtr);
                    break;
                case EPOLLRDHUP:
                    break;
                case EPOLLHUP:
                    handleHangUp(ePollDataPtr);
                    break;
                case EPOLLERR:
                    break;
                default:
                    break;
                }
            }
            catch (const std::exception &e)
            {
                // statuc code err
                // runtime err
                std::cerr << e.what() << '\n';
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
        throw std::runtime_error("usage: webserv [path to config]\n\n\n");
    spdlog::set_level(spdlog::level::debug);

    Configuration config{};
    if (initConfig(argv[1], config))
        return 1;

    run(config);

    return 0;
}