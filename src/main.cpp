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
        spdlog::debug("response = \n{}", client->m_request.m_response);
        if (client->m_request.m_response.sendAll(client->m_socketFd, client->m_server.m_serverconfig.getErrorPagesConfig()) <= 0)
        {
            multiplexer.removeFromEpoll(client->m_socketFd); //refactor what if we throw error here do we get a loop?
            multiplexer.removeClientBySocketFd(client);
        }
    }
    else if (CGIPipeIn *pipein = dynamic_cast<CGIPipeIn *>(ePollDataPtr))
    {
        multiplexer.removeFromEpoll(pipein->m_pipeFd[WRITE]);
        pipein->dupCloseWrite();
        if (multiplexer.addToEpoll(&(pipein->m_client.m_request.m_pipeout), EPOLLIN, pipein->m_client.m_request.m_pipeout.m_pipeFd[READ]) == -1)
            throw StatusCodeException(500, "addToEpoll()", errno);
        pipein->m_client.m_request.m_pipeout.forkCloseDupExec();
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
                    multiplexer.removeFromEpoll(ePollDataPtr->m_socketFd);
                    throw StatusCodeException(500, "EPOLLHUP", errno);
                case EPOLLERR:
                    break;
                default:
                    break;
                }
            }
            catch (const StatusCodeException &e)
            {
                spdlog::critical("CAUGHT ERROR");
                if (Client *client = dynamic_cast<Client *>(ePollDataPtr))
                {
                    std::cerr << e.what() << '\n';
                    client->m_request.m_response.m_statusCode = e.getStatusCode();
                    multiplexer.modifyEpollEvents(client, EPOLLOUT, client->m_socketFd);
                }
                else if (CGIPipeOut *pipeout = dynamic_cast<CGIPipeOut *>(ePollDataPtr))
                {
                    std::cerr << e.what() << '\n';
                    pipeout->m_client.m_request.m_response.m_statusCode = e.getStatusCode();
                    multiplexer.modifyEpollEvents(&pipeout->m_client, EPOLLOUT, pipeout->m_client.m_socketFd);
                }
                else if (CGIPipeIn *pipein = dynamic_cast<CGIPipeIn *>(ePollDataPtr))
                {
                    std::cerr << e.what() << '\n';
                    pipein->m_client.m_request.m_response.m_statusCode = e.getStatusCode();
                    multiplexer.modifyEpollEvents(&pipein->m_client, EPOLLOUT, pipein->m_client.m_socketFd);
                }
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