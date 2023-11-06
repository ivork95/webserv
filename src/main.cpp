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

#define PARSTER false // change this

void handleRead(ASocket *&ePollDataPtr, std::vector<ASocket *> &toBeDeleted)
{
    Multiplexer &multiplexer = Multiplexer::getInstance();

    if (Client *client = dynamic_cast<Client *>(ePollDataPtr)) // Client is ready to read, handle incoming data
        client->handleConnectedClient(toBeDeleted);
    else if (Server *server = dynamic_cast<Server *>(ePollDataPtr)) // Server is ready to read, handle new connection
        server->handleNewConnection();
    else if (CGIPipeOut *pipeout = dynamic_cast<CGIPipeOut *>(ePollDataPtr))
    {
        char buf[BUFSIZ]{};

        int nbytes{static_cast<int>(read(pipeout->m_pipeFd[READ], &buf, BUFSIZ - 1))};
        if (nbytes <= 0)
            pipeout->m_response.m_statusCode = 500;
        else
        {
            pipeout->m_response.m_body = buf;
            pipeout->m_response.m_statusCode = 200;
        }

        close(pipeout->m_pipeFd[READ]);
        pipeout->m_socketFd = -1;
        toBeDeleted.push_back(pipeout);

        if (multiplexer.modifyEpollEvents(&pipeout->m_client, EPOLLOUT, pipeout->m_client.m_socketFd))
            throw std::system_error(errno, std::generic_category(), "modifyEpollEvents()");
    }
    else if (Timer *timer = dynamic_cast<Timer *>(ePollDataPtr))
    {
        close(timer->m_socketFd);
        timer->m_socketFd = -1;

        close(timer->m_client.m_socketFd);
        timer->m_client.m_socketFd = -1;
        toBeDeleted.push_back(&(timer->m_client));
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
            std::cout << "Got SIGINT" << std::endl;

            for (auto &server : multiplexer.m_servers)
                delete server;
            for (auto &s : toBeDeleted)
                delete s;
            toBeDeleted.clear();
            multiplexer.isRunning = false;
        }
        else if (fdsi.ssi_signo == SIGQUIT)
            std::cout << "Got SIGQUIT" << std::endl;
        else
            std::cout << "Read unexpected signal" << std::endl;
    }
}

void handleWrite(ASocket *&ePollDataPtr, std::vector<ASocket *> &toBeDeleted)
{
    Multiplexer &multiplexer = Multiplexer::getInstance();

    if (Client *client = dynamic_cast<Client *>(ePollDataPtr))
    {
        if (client->m_request.m_response.sendAll(client->m_socketFd) <= 0)
        {
            close(client->m_socketFd);
            client->m_socketFd = -1;
            toBeDeleted.push_back(client);
        }
    }
    else if (CGIPipeIn *pipein = dynamic_cast<CGIPipeIn *>(ePollDataPtr))
    {
        try
        {
            pipein->dupCloseWrite(toBeDeleted);

            CGIPipeOut *pipeout = new CGIPipeOut(pipein->m_client, pipein->m_client.m_request, pipein->m_client.m_request.m_response);
            if (multiplexer.addToEpoll(pipeout, EPOLLIN, pipeout->m_pipeFd[READ]))
                throw StatusCodeException(500, "addToEpoll()", errno);

            pipeout->forkCloseDupExec(toBeDeleted);
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

    for (auto &serverConfig : config.serversConfig)
        multiplexer.m_servers.push_back(new Server{serverConfig});

    std::vector<ASocket *> toBeDeleted{};
    while (multiplexer.isRunning)
    {
        for (auto &s : toBeDeleted)
            delete s;
        toBeDeleted.clear();

        epollCount = epoll_wait(multiplexer.m_epollfd, multiplexer.m_events.data(), MAX_EVENTS, -1);
        if (epollCount == -1)
            throw std::system_error(errno, std::generic_category(), "epoll_wait()");

        for (i = 0; i < epollCount; i++) // Loop through ready list
        {
            ASocket *ePollDataPtr = static_cast<ASocket *>(multiplexer.m_events[i].data.ptr);

            if (ePollDataPtr->m_socketFd == -1)
                continue;

            if (multiplexer.m_events[i].events & EPOLLIN) // Ready to read
                handleRead(ePollDataPtr, toBeDeleted);
            else if (multiplexer.m_events[i].events & EPOLLOUT) // Ready to write
                handleWrite(ePollDataPtr, toBeDeleted);
            else if (multiplexer.m_events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) // Ready to hang up/error
            {
                std::cout << "Ready to hang up/error\n";

                close(ePollDataPtr->m_socketFd);
                ePollDataPtr->m_socketFd = -1;
                toBeDeleted.push_back(ePollDataPtr);
            }
            else // Ready for something else
                std::cout << "Other event ready\n";
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
        throw std::runtime_error("usage: webserv [path to config]\n\n\n");

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