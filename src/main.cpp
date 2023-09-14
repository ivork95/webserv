#include <csignal>
#include "TcpServer.hpp"
#include "Client.hpp"
#include "MultiplexerIO.hpp"
#include "HttpRequest.hpp"
#include <fstream>
#include "Timer.hpp"
#include "HttpResponse.hpp"

#define BUFSIZE 256

void handleConnectedClient(Client *client)
{
    char buf[BUFSIZE + 1]{}; // Buffer for client data
    int nbytes = recv(client->m_socketFd, buf, BUFSIZE, 0);

    if (timerfd_settime(client->timer->m_socketFd, 0, &client->timer->m_spec, NULL) == -1)
    {
        std::perror("timerfd_settime()");
        throw std::runtime_error("Error: timerfd_settime()\n");
    }

    if (nbytes <= 0) // Got error or connection closed by client
    {
        if (nbytes == 0) // Connection closed
            std::cout << "socket " << *client << " hung up\n";
        else
            std::cerr << "Error: recv() failed\n";
    }
    else // We got some good data from a client
    {
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
            if (!client->httpRequest.isContentLengthConverted)
            {
                client->httpRequest.setContentLength();
            }
            if (client->httpRequest.m_contentLength > static_cast<int>((client->httpRequest.m_rawMessage.length() - (fieldLinesEndPos + 4))))
            {
                spdlog::warn("Content-Length not reached [...]");
                return;
            }
            spdlog::info("Content-Length reached!");
            spdlog::info("Content-Length = {}", client->httpRequest.m_contentLength);
            spdlog::info("Body length = {}", client->httpRequest.m_rawMessage.length() - (fieldLinesEndPos + 4));
        }

        spdlog::info("message complete!");
        client->httpRequest.setMethodPathVersion();
        spdlog::info("httprequest = {}", client->httpRequest);

        /*
        A server MUST respond with a 400 (Bad Request) status code to any HTTP/1.1 request message that lacks a Host header field and to any request message that contains more than one Host header field line or a Host header field with an invalid field value.
        */

        HttpResponse httpresponse{};
        if (!client->httpRequest.m_methodPathVersion[0].compare("GET"))
        {
            spdlog::debug("GET method");

            std::string path{client->httpRequest.m_methodPathVersion[1]};
            if (!path.compare("/"))
                path = "./www/index.html";
            else
            {
                path = "./www" + client->httpRequest.m_methodPathVersion[1] + ".html";
            }

            std::ifstream inf(path);
            if (!inf)
            {
                httpresponse.m_statusLine = "HTTP/1.1 404 NOT FOUND";
                std::string s{httpresponse.responseBuild()};
                send(client->m_socketFd, s.data(), s.length(), 0);
                delete (client);
                return;
            }

            int n{};
            while (inf)
            {
                std::string strInput;
                std::getline(inf, strInput);
                httpresponse.m_body.append(strInput);
                n = n + strInput.length();
            }

            std::string s{httpresponse.responseBuild()};
            send(client->m_socketFd, s.data(), s.length(), 0);
            delete client;
            return;
        }
        else if (!client->httpRequest.m_methodPathVersion[0].compare("POST"))
        {
            spdlog::info("POST method");

            if (!client->httpRequest.m_requestHeaders.contains("Content-Length"))
            {
                httpresponse.m_statusLine = "HTTP/1.1 411 Length Required";
                std::string s{httpresponse.responseBuild()};
                send(client->m_socketFd, s.data(), s.length(), 0);
                delete (client);
                return;
            }

            if (client->httpRequest.m_contentLength > client->httpRequest.m_client_max_body_size)
            {
                httpresponse.m_statusLine = "HTTP/1.1 413 Payload Too Large";
                std::string s{httpresponse.responseBuild()};
                send(client->m_socketFd, s.data(), s.length(), 0);
                delete (client);
                return;
            }

            spdlog::info("m_rawMessage = {}", client->httpRequest.m_rawMessage);

            client->httpRequest.setBoundaryCode();
            client->httpRequest.setGeneralHeaders();
            client->httpRequest.setFileName();
            client->httpRequest.setBody();
            spdlog::info(client->httpRequest);

            std::string s{httpresponse.responseBuild()};
            send(client->m_socketFd, s.data(), s.length(), 0);
            delete client;
            return;
        }
        else if (!client->httpRequest.m_methodPathVersion[0].compare("DELETE"))
        {
            spdlog::debug("DELETE method");
        }
        else
        {
            spdlog::debug("Method not allowed");
        }
        // spdlog::debug("{}", httpresponse);
        // httpresponse.m_headers.insert(std::make_pair("Content-Length", std::to_string(n)));
        // httpresponse.m_headers.insert(std::make_pair("Content-Type", "text/html"));
        // std::string httpResponse = httpresponse.responseBuild();

        // std::cout << "Content-length value: " << client->httpRequest.m_contentLength << std::endl;
        // std::cout << "body-lenght value: " << client->httpRequest.m_rawMessage.length() - (client->httpRequest.m_fieldLinesEndPos + 4) << std::endl;

        // send(client->m_socketFd, httpResponse.data(), httpResponse.length(), 0);
    }
    delete client;
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
        if (epollCount <= 0)
        {
            if (epollCount == 0)
            {
                std::perror("no file descriptor became ready during the requested timeout");
                throw std::runtime_error("Error: no file descriptor became ready during the requested timeout\n");
            }
            else
            {
                std::perror("epoll_wait() failed");
                throw std::runtime_error("Error: epoll_wait() failed\n");
            }
        }

        for (int i{0}; i < epollCount; i++) // Run through the existing connections looking for data to read
        {
            Socket *ePollDataPtr{static_cast<Socket *>(multiplexerio.m_events.at(i).data.ptr)};

            // Check if someone's ready to read
            if (multiplexerio.m_events.at(i).events & EPOLLIN) // We got one!!
            {
                if (TcpServer *server = dynamic_cast<TcpServer *>(ePollDataPtr))
                // If listener is ready to read, handle new connection
                {
                    Client *client = new Client{*server};
                    multiplexerio.addSocketToEpollFd(client, EPOLLIN | EPOLLOUT);
                    multiplexerio.addSocketToEpollFd(client->timer, EPOLLIN);
                }
                else if (Timer *timer = dynamic_cast<Timer *>(ePollDataPtr))
                {
                    spdlog::warn("Timeout expired. Closing: {}", *(timer->m_client));
                    delete timer->m_client;
                }
                else // If not the listener, we're just a regular client
                    handleConnectedClient(dynamic_cast<Client *>(ePollDataPtr));
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
