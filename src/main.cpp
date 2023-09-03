#include <csignal>
#include "TcpServer.hpp"
#include "Client.hpp"
#include "MultiplexerIO.hpp"
#include "HttpMessage.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <fstream>

#define BUFSIZE 256

// HttpResponse, misschien HttpResponse laten inheriten van HttpMessage?
std::string createFilePath(std::string path)
{
    for (char c : path)
    {
        if (c != '/')
            return ("./www" + path + ".html");
    }

    return ("./www/index.html");
}

void sendResponse(HttpResponse response, int fd)
{
    if (response.getStatusCode() == 0)
    {
        std::cerr << "Error: Couldn't send response, not a valid status code" << std::endl;
        return;
    }
    std::string res{response.generateResponse()};
    const void *dataPointer = static_cast<const void *>(res.c_str());
    std::cout << "Response generated" << std::endl;
    send(fd, dataPointer, res.length(), 0);
}

HttpResponse createResponse(HttpRequest request)
{
    HttpResponse response;

    if (request.getMethod() == GET)
    {
        std::string path = createFilePath(request.getPath());
        std::cout << "FILEPATH: " << path << "\n\n";
        std::ifstream file(path);

        if (!file.is_open())
        {
            file.open("./www/404.html");
            if (!file.is_open())
                throw std::runtime_error("Error: Could not open default error page");
            response.setStatus(404, "Not Found");
        }
        else
            response.setStatus(200, "OK");
        response.setHtmlContent(file);
        file.close();
    }
    else if (request.getMethod() == POST)
    {
        std::map<std::string, std::string> fileExtension =
            {
                {"text/plain", "txt"},
                {"application/json", "json"},
                {"image/png", "png"},
                //  TODO add all file extensions
                //  define in header
            };
        // read content type
        auto it = request.getHeaders().find("Content-Type");
        if (it == request.getHeaders().end())
        {
            std::cout << "Error finding file type" << std::endl;
            // handle this error properly
            return response;
        }
        std::cout << "\n\nCONTENT-TYPE: " << it->second << "\n\n";
        std::cout << "\n\nPATH: " << request.getPath() << "\n\n";
        std::string filePath = "/www" + request.getPath() + fileExtension[it->second];
        std::ofstream outputFile(filePath, std::ios::out | std::ios::binary);
        outputFile << request.getBody();
        response.setStatus(200, "OK");
        response.setHeader("Content-Type", "text/html");
        response.setBody("Data saved!");
    }
    return response;
}

void handleConnectedClient(Client *client)
{
    char buf[BUFSIZE + 1]{}; // Buffer for client data

    int nbytes = recv(client->m_socketFd, buf, BUFSIZE, 0);
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
        spdlog::info("buf = \n|{}|", buf);

        client->m_rawMessage.append(buf);
        spdlog::info("client->m_rawMessage = \n|{}|", client->m_rawMessage);

        size_t headersEndPos = client->m_rawMessage.find("\r\n\r\n");
        if (headersEndPos == std::string::npos)
        {
            spdlog::warn("message incomplete [...]");
            return;
        }

        if (client->m_method.empty())
        {
            client->setMethodPathVersion(); // redo function with clear input and output
            client->setHeaders();           // redo function with clear input and output
        }

        if (!client->m_method.compare("POST"))
        {
            spdlog::info("POST method");

            if (client->m_headers.find("Content-Length") == client->m_headers.end())
            {
                spdlog::warn("411 Length Required");
                close(client->m_socketFd);
                delete client;
            }

            if (!client->m_isStoiCalled)
            {
                spdlog::info("std::stoi()");
                try
                    client->setContentLength();
                catch (...)
                {
                    spdlog::warn("400 Bad Request");
                    close(client->m_socketFd);
                    delete client;
                }
            }

            if (client->m_contentLength > client->m_client_max_body_size)
            {
                spdlog::warn("413 Request Entity Too Large");
                close(client->m_socketFd);
                delete client;
            }

            if (client->m_contentLength > (client->m_rawMessage.length() - (headersEndPos + 4)))
            {
                spdlog::warn("Content-Length not reached [...]");
                return;
            }
            spdlog::info("Content-Length reached!");
        }
        else
        {
            spdlog::info("GET or OPTIONS method");
        }
    }
    close(client->m_socketFd);
    delete client;
}

void run(int argc, char *argv[])
{
    std::vector<TcpServer *> servers{};
    for (int i{1}; i < argc; i++)
        servers.push_back(new TcpServer{argv[i]});

    MultiplexerIO multiplexerio{};
    for (auto &server : servers)
        multiplexerio.addSocketToEpollFd(server);

    while (true)
    {
        int epollCount{epoll_wait(multiplexerio.m_epollfd, multiplexerio.m_events.data(), MAX_EVENTS, (60 * 1000 * 3))};
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
                    Client *clientsocket = new Client{*server};
                    multiplexerio.addSocketToEpollFd(clientsocket);
                }
                else // If not the listener, we're just a regular client
                    handleConnectedClient(dynamic_cast<Client *>(ePollDataPtr));
            }
        }
    }
}

int main(int argc, char *argv[])
{
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
