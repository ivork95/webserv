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
#include "Cgi.hpp"

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
        if (client->httpRequest.tokenize(buf, nbytes))
            return;

        if (timerfd_settime(client->timer->m_socketFd, 0, &client->timer->m_spec, NULL) == -1)
            throw StatusCodeException(500, "Error: timerfd_settime()");

        client->httpRequest.parse();
    }
    catch (const StatusCodeException &e)
    {
        client->httpRequest.m_statusCode = e.getStatusCode();
        spdlog::warn(e.what());
    }
    spdlog::critical(client->httpRequest);
    client->isWriteReady = true;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
        throw std::runtime_error("usage: webserv [port]\n\n\n");

    Configuration config{};
    if (initConfig(argv[1], config))
    {
        std::cout << "Config failure" << std::endl;
        return 1;
    }

#if (PARSTER) // To run only the parser and display the output
    std::cout << "\n\t\t -----------------\n \t\t[  SERVER CONFIG  ]\n\t\t -----------------\n";
    for (size_t i = 0; i < config.serversConfig.size(); ++i)
    {
        std::cout << config.serversConfig[i];
    }
    return 0;
#endif

    spdlog::set_level(spdlog::level::debug);

    std::vector<TcpServer *> servers{};
    for (auto &serverConfig : config.serversConfig)
    {
        spdlog::critical("YOO");
        servers.push_back(new TcpServer{serverConfig.getPortNb().c_str()});
    }

    MultiplexerIO &multiplexerio = MultiplexerIO::getInstance();
    multiplexerio.run(servers);

    return 0;
}