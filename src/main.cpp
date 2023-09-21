#include <csignal>
#include <fstream>
#include <filesystem>
#include <sys/stat.h>
#include "TcpServer.hpp"
#include "Client.hpp"
#include "MultiplexerIO.hpp"
#include "HttpRequest.hpp"
#include "Timer.hpp"
#include "HttpResponse.hpp"
#include "Cgi.hpp"

#define BUFSIZE 256

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
        if (timerfd_settime(client->timer->m_socketFd, 0, &client->timer->m_spec, NULL) == -1)
            throw StatusCodeException(500, "Error: timerfd_settime()");

        // We got some good data from a client
        spdlog::info("nbytes = {}", nbytes);

        client->httpRequest.m_rawMessage.append(buf, buf + nbytes);

        size_t fieldLinesEndPos = client->httpRequest.m_rawMessage.find("\r\n\r\n");
        if (fieldLinesEndPos == std::string::npos)
        {
            spdlog::warn("message incomplete [...]");
            return;
        }

        if (client->httpRequest.m_requestHeaders.empty())
            client->httpRequest.setRequestHeaders();

        if (client->httpRequest.m_requestHeaders.contains("Content-Length"))
        {
            if (!client->httpRequest.m_isContentLengthConverted)
                client->httpRequest.setContentLength();
            if (client->httpRequest.m_contentLength > static_cast<int>((client->httpRequest.m_rawMessage.length() - (fieldLinesEndPos + 4))))
            {
                spdlog::warn("Content-Length not reached [...]");
                return;
            }
            spdlog::info("Content-Length reached!");
        }

        if (client->httpRequest.m_contentLength > client->httpRequest.m_client_max_body_size)
            throw StatusCodeException(413, "Warning: contentLength larger than max_body_size");

        spdlog::info("message complete!");
        spdlog::info("client->httpRequest.m_rawMessage = \n|{}|", client->httpRequest.m_rawMessage);

        client->httpRequest.setMethodPathVersion();

        if (!client->httpRequest.m_methodPathVersion[0].compare("POST"))
        {
            client->httpRequest.setBoundaryCode();
            client->httpRequest.setGeneralHeaders();
            client->httpRequest.setFileName();
            client->httpRequest.setBody();
            client->httpRequest.bodyToDisk("./www/" + client->httpRequest.m_fileName);
        }
        client->httpRequest.m_statusCode = 200;
    }
    catch (const StatusCodeException &e)
    {
        client->httpRequest.m_statusCode = e.getStatusCode();
        spdlog::warn(e.what());
    }
    spdlog::critical(client->httpRequest);

    MultiplexerIO &multiplexerio = MultiplexerIO::getInstance();
    multiplexerio.modifyEpollEvents(client, EPOLLOUT);
}

void run(int argc, char *argv[])
{
    std::vector<TcpServer *> servers{};
    for (int i{1}; i < argc; i++)
        servers.push_back(new TcpServer{argv[i]});

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

            if (multiplexerio.m_events.at(i).events & EPOLLIN) // If someone's ready to read
            {
                if (TcpServer *server = dynamic_cast<TcpServer *>(ePollDataPtr)) // If listener is ready to read, handle new connection
                {
                    Client *client = new Client{*server};
                    multiplexerio.addSocketToEpollFd(client, EPOLLIN);
                    multiplexerio.addSocketToEpollFd(client->timer, EPOLLIN);
                }
                else if (Timer *timer = dynamic_cast<Timer *>(ePollDataPtr))
                {
                    spdlog::warn("Timeout expired. Closing: {}", *(timer->m_client));
                    delete timer->m_client;
                }
                else if (Client *client = dynamic_cast<Client *>(ePollDataPtr)) // If not the listener, we're just a regular client
                {
                    handleConnectedClient(client);
                }
            }
            else if (multiplexerio.m_events.at(i).events & EPOLLOUT) // If someone's ready to write
            {
                if (Client *client = dynamic_cast<Client *>(ePollDataPtr))
                {
                    HttpResponse response{client->httpRequest};
                    response.responseHandle();
                    spdlog::critical(response);

                    std::string s{response.responseBuild()};
                    spdlog::critical("s = |{}|", s);
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
    /* Catch-all handler
        try
        {
            runGame();
        }
        catch(...)
        {
            std::cerr << "Abnormal termination\n";
        }
    */
    if (argc < 2)
        throw std::runtime_error("usage: webserv [port]\n\n\n");

    run(argc, argv);

    return 0;
}
