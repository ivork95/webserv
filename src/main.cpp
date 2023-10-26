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

#define PARSTER false // change this

void run(const Configuration &config)
{
    std::vector<Server *> servers{};

    for (auto &serverConfig : config.serversConfig)
        servers.push_back(new Server{serverConfig});

    Multiplexer &multiplexer = Multiplexer::getInstance();
    for (auto &server : servers)
    {
        if (multiplexer.addToEpoll(server, EPOLLIN | EPOLLRDHUP, server->m_socketFd))
            throw std::runtime_error("Error: addToEpoll() failed\n");
    }

    while (true)
    {
        std::vector<Socket *> toBeDeleted{};

        for (auto &s : toBeDeleted)
            delete s;

        int epollCount = epoll_wait(multiplexer.m_epollfd, multiplexer.m_events.data(), MAX_EVENTS, -1);
        if (epollCount < 0)
            throw std::runtime_error("Error: epoll_wait() failed\n");

        for (int i = 0; i < epollCount; i++) // Run through the existing connections looking for data to read
        {
            Socket *ePollDataPtr = static_cast<Socket *>(multiplexer.m_events[i].data.ptr);

            if (ePollDataPtr->m_socketFd == -1)
                continue;

            if (multiplexer.m_events[i].events & EPOLLIN) // If someone's ready to read
            {
                if (Client *client = dynamic_cast<Client *>(ePollDataPtr)) // If not the listener, we're just a regular client
                    client->handleConnectedClient(toBeDeleted);
                else if (Server *server = dynamic_cast<Server *>(ePollDataPtr)) // If listener is ready to read, handle new connection
                {
                    Client *client = new Client{*server};
                    if (multiplexer.addToEpoll(client, EPOLLIN | EPOLLRDHUP, client->m_socketFd))
                        throw std::runtime_error("Error: addToEpoll()\n");
                    if (multiplexer.addToEpoll(&(client->m_timer), EPOLLIN | EPOLLRDHUP, client->m_timer.m_socketFd))
                        throw std::runtime_error("Error: addToEpoll()\n");
                }
                else if (CGIPipeOut *pipeout = dynamic_cast<CGIPipeOut *>(ePollDataPtr))
                {
                    spdlog::critical("cgi out READ ready!");

                    char buf[BUFSIZ]{};
                    int nbytes{static_cast<int>(read(pipeout->m_pipeFd[READ], &buf, BUFSIZ - 1))};
                    if (nbytes <= 0)
                        pipeout->m_response.m_statusCode = 500;
                    else
                    {
                        pipeout->m_response.m_body = buf;
                        pipeout->m_response.m_statusCode = 200;
                    }
                    if (multiplexer.modifyEpollEvents(&pipeout->m_client, EPOLLOUT, pipeout->m_client.m_socketFd))
                        throw std::runtime_error("Error: modifyEpollEvents()\n");
                }
                else if (Timer *timer = dynamic_cast<Timer *>(ePollDataPtr))
                {
                    spdlog::warn("Timeout expired. Closing: {}", timer->m_client);

                    close(timer->m_client.m_socketFd);
                    timer->m_client.m_socketFd = -1;
                    toBeDeleted.push_back(&(timer->m_client));

                    close(timer->m_socketFd);
                    timer->m_socketFd = -1;
                }
            }
            else if (multiplexer.m_events[i].events & EPOLLOUT) // If someone's ready to write
            {
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
                    spdlog::critical("cgi in WRITE ready!");

                    try
                    {
                        pipein->dupCloseWrite(toBeDeleted);

                        // Create pipeout
                        CGIPipeOut *pipeout = new CGIPipeOut(pipein->m_client, pipein->m_client.m_request, pipein->m_client.m_request.m_response);
                        if (pipe(pipeout->m_pipeFd) == -1)
                            throw StatusCodeException(500, "Error: pipe()");
                        if (Helper::setNonBlocking(pipeout->m_pipeFd[READ] == -1))
                            throw StatusCodeException(500, "Error: setNonBlocking()");
                        if (Helper::setNonBlocking(pipeout->m_pipeFd[WRITE] == -1))
                            throw StatusCodeException(500, "Error: setNonBlocking()");
                        if (multiplexer.addToEpoll(pipeout, EPOLLIN, pipeout->m_pipeFd[READ])) // Add READ end from pipe2 to epoll
                            throw StatusCodeException(500, "Error: epoll_ctl()");

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
            else
                spdlog::critical("Unexpected");
            if (multiplexer.m_events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
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