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
    {
        // spdlog::error("pipeout->m_pipefd[1]: {}", pipeout->m_pipeFd[WRITE]);
        // spdlog::error("pipeout->m_pipefd[0]: {}", pipeout->m_pipeFd[READ]);
        char buf[BUFSIZ]{};

        int nbytes{static_cast<int>(read(pipeout->m_pipeFd[READ], &buf, BUFSIZ - 1))};
        if (nbytes <= 0)
            pipeout->m_response.m_statusCode = 500;
        else
        {
            pipeout->m_response.m_body = buf;
            pipeout->m_response.m_statusCode = 200;
        }

        if(epoll_ctl(multiplexer.m_epollfd, EPOLL_CTL_DEL, pipeout->m_pipeFd[READ], NULL) == -1)
            throw std::system_error(errno, std::generic_category(), "epoll_ctl()");
        // close(pipeout->m_pipeFd[READ]);

        if (multiplexer.modifyEpollEvents(&pipeout->m_client, EPOLLOUT, pipeout->m_client.m_socketFd))
            throw std::system_error(errno, std::generic_category(), "modifyEpollEvents()");
    }
    else if (Timer *timer = dynamic_cast<Timer *>(ePollDataPtr))
    {
        multiplexer.modifyEpollEvents(nullptr, 0, timer->m_socketFd);
        epoll_ctl(multiplexer.m_epollfd, EPOLL_CTL_DEL, timer->m_socketFd, NULL);
        multiplexer.modifyEpollEvents(nullptr, 0, timer->m_client.m_socketFd);
        epoll_ctl(multiplexer.m_epollfd, EPOLL_CTL_DEL, timer->m_client.m_socketFd, NULL);
        delete &timer->m_client;
    }
    else if (Signal *signal = dynamic_cast<Signal *>(ePollDataPtr))
    {
        struct signalfd_siginfo fdsi
        {
        };

        ssize_t s = read(signal->m_socketFd, &fdsi, sizeof(fdsi));
        if (s != sizeof(fdsi))
            throw std::system_error(errno, std::generic_category(), "read()");
        if (fdsi.ssi_signo == SIGINT)
        {
            for (auto &server : multiplexer.m_servers)
                delete server;
            multiplexer.isRunning = false;
        }
        else if (fdsi.ssi_signo == SIGQUIT)
            ;
        else
            ;
    }
}

void handleWrite(ASocket *&ePollDataPtr)
{
    Multiplexer &multiplexer = Multiplexer::getInstance();

    if (Client *client = dynamic_cast<Client *>(ePollDataPtr))
    {
        spdlog::debug("response = \n{}", client->m_request.m_response);
        if (client->m_request.m_response.sendAll(client->m_socketFd) <= 0)
        {
            multiplexer.modifyEpollEvents(nullptr, 0, client->m_socketFd);
            epoll_ctl(multiplexer.m_epollfd, EPOLL_CTL_DEL, client->m_socketFd, NULL);
            delete client;
        }
    }
    else if (CGIPipeIn *pipein = dynamic_cast<CGIPipeIn *>(ePollDataPtr))
    {
        try
        {
            // spdlog::error("pipein->m_pipefd: {}", pipein->m_pipeFd[WRITE]);
            pipein->dupCloseWrite();

            if (multiplexer.addToEpoll(&(pipein->m_client.m_request.m_pipeout), EPOLLIN, pipein->m_client.m_request.m_pipeout.m_pipeFd[READ]))
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

    try
    {
        for (auto &serverConfig : config.serversConfig)
            multiplexer.m_servers.push_back(new Server{serverConfig});
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return;
    }

    while (multiplexer.isRunning)
    {
        epollCount = epoll_wait(multiplexer.m_epollfd, multiplexer.m_events.data(), MAX_EVENTS, -1);
        if (epollCount == -1)
            throw std::system_error(errno, std::generic_category(), "epoll_wait()");

        for (i = 0; i < epollCount; i++) // Loop through ready list
        {
            ASocket *ePollDataPtr = static_cast<ASocket *>(multiplexer.m_events[i].data.ptr);

            if (ePollDataPtr == nullptr)
            {
                spdlog::critical("INTERESTING");
                continue;
            }

            if (multiplexer.m_events[i].events & EPOLLIN) // Ready to read
                handleRead(ePollDataPtr);
            else if (multiplexer.m_events[i].events & EPOLLOUT) // Ready to write
                handleWrite(ePollDataPtr);
            else if (multiplexer.m_events[i].events & EPOLLRDHUP) // Ready to hang up/error
            {
                spdlog::warn("EPOLLRDHUP");
            }
            else if (multiplexer.m_events[i].events & (EPOLLHUP)) // Ready to hang up/error
            {
                spdlog::warn("EPOLLHUP");
                // spdlog::error("ePollDataPtr->m_socketFd: {}", ePollDataPtr->m_socketFd);
                // multiplexer.removeFromEpoll(ePollDataPtr->m_socketFd);
                // throw std::runtime_error("EPHOLLHUP");
            }
            else if (multiplexer.m_events[i].events & (EPOLLERR)) // Ready to hang up/error
            {
                spdlog::warn("EPOLLERR");
            }
            else // Ready for something else
            {
                spdlog::info("SOMETHING ELSE");
            }
        }
    }
}

int main(int argc, char *argv[])
{
    spdlog::set_level(spdlog::level::debug);

    if (argc != 2)
        throw std::runtime_error("usage: webserv [path to config]\n\n\n");

    Configuration config{};
    if (initConfig(argv[1], config))
    {
        return 1;
    }

    config.printConfig();

#if (PARSTER) // To run only the parser and display the output
    return 0;
#endif

    run(config);

    return 0;
}