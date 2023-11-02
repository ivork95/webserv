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

void handleRead(Socket *&ePollDataPtr, std::vector<Socket *> &toBeDeleted)
{
	Logger &logger = Logger::getInstance();

    Multiplexer &multiplexer = Multiplexer::getInstance();

    if (Client *client = dynamic_cast<Client *>(ePollDataPtr)) // Client is ready to read, handle incoming data
        client->handleConnectedClient(toBeDeleted);
    else if (Server *server = dynamic_cast<Server *>(ePollDataPtr)) // Server is ready to read, handle new connection
        server->handleNewConnection();
    else if (CGIPipeOut *pipeout = dynamic_cast<CGIPipeOut *>(ePollDataPtr))
    {
        // spdlog::info("cgi out READ ready!");
		logger.debug("cgi out READ ready!");

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
            throw std::runtime_error("Error: modifyEpollEvents()\n");
    }
    else if (Timer *timer = dynamic_cast<Timer *>(ePollDataPtr))
    {
        // spdlog::warn("Timeout expired. Closing: {}", timer->m_client);
		logger.debug("Timeout expired. Closing: Client(" + std::to_string(timer->m_client.m_socketFd) + ": " + timer->m_client.m_ipver + ": " + timer->m_client.m_ipstr + ": " + std::to_string(timer->m_client.m_port));

        close(timer->m_socketFd);
        timer->m_socketFd = -1;

        close(timer->m_client.m_socketFd);
        timer->m_client.m_socketFd = -1;
        toBeDeleted.push_back(&(timer->m_client));
    }
}

void handleWrite(Socket *&ePollDataPtr, std::vector<Socket *> &toBeDeleted)
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
        // spdlog::critical("cgi in WRITE ready!");
		Logger::getInstance().debug("cgi in WRITE ready!");

        try
        {
            pipein->dupCloseWrite(toBeDeleted);

            CGIPipeOut *pipeout = new CGIPipeOut(pipein->m_client, pipein->m_client.m_request, pipein->m_client.m_request.m_response);
            if (multiplexer.addToEpoll(pipeout, EPOLLIN, pipeout->m_pipeFd[READ])) // Add READ end from pipe2 to epoll
                throw StatusCodeException(500, "Error: epoll_ctl()");

            pipeout->forkCloseDupExec(toBeDeleted);
        }
        catch (const StatusCodeException &e)
        {
            // std::cerr << e.what() << '\n';
			Logger::getInstance().error(e.what());
            pipein->m_client.m_request.m_response.m_statusCode = e.getStatusCode();
            multiplexer.modifyEpollEvents(&(pipein->m_client), EPOLLOUT, pipein->m_client.m_socketFd);
        }
    }
}

void run(const Configuration &config)
{
	Logger &logger = Logger::getInstance();

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

        for (int i = 0; i < epollCount; i++) // Loop through ready list
        {
            Socket *ePollDataPtr = static_cast<Socket *>(multiplexer.m_events[i].data.ptr);

            if (ePollDataPtr->m_socketFd == -1)
                continue;

            if (multiplexer.m_events[i].events & EPOLLIN) // Ready to read
                handleRead(ePollDataPtr, toBeDeleted);
            else if (multiplexer.m_events[i].events & EPOLLOUT) // Ready to write
                handleWrite(ePollDataPtr, toBeDeleted);
            else if (multiplexer.m_events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) // Ready to hang up/error
            {
                // spdlog::info("Ready to hang up/error");
				logger.debug("Ready to hang up/error");

                if (Server *server = dynamic_cast<Server *>(ePollDataPtr)) {
                    // spdlog::info("A");
					logger.debug("A");
				}

                if (Client *client = dynamic_cast<Client *>(ePollDataPtr)) {
                    // spdlog::info("B");
					logger.debug("B");
				}

                if (Timer *timer = dynamic_cast<Timer *>(ePollDataPtr)) {
                    // spdlog::info("C");
					logger.debug("C");
				}

                if (CGIPipeIn *in = dynamic_cast<CGIPipeIn *>(ePollDataPtr)) {
                    // spdlog::info("D");
					logger.debug("D");
				}

                if (CGIPipeOut *out = dynamic_cast<CGIPipeOut *>(ePollDataPtr)) {
                    // spdlog::info("E");
					logger.debug("E");
				}

                close(ePollDataPtr->m_socketFd);
                ePollDataPtr->m_socketFd = -1;
                toBeDeleted.push_back(ePollDataPtr);
            }
            else // Ready for something else
                // spdlog::critical("Other event ready");
				logger.warn("Other event ready");
        }
    }
}

int main(int argc, char *argv[])
{
    // spdlog::set_level(spdlog::level::debug);

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