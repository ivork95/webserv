#include <csignal>
#include "../include/TcpServer.hpp"
#include "../include/Client.hpp"
#include "../include/MultiplexerIO.hpp"
#include "../include/HttpMessage.hpp"
#include "../include/HttpRequest.hpp"
#include "../include/HttpResponse.hpp"
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

//
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

void do_use_fd(Socket *ePollDataPtr)
{
    Client *client{};

    if ((client = dynamic_cast<Client *>(ePollDataPtr)))
        ;
    else
    {
        std::cerr << "Error: the actual type of the object pointed to by ePollDataPtr = Unrecognized\n";
        return;
    }

    char buf[BUFSIZE + 1]{}; // Buffer for client data
    int nbytes = recv(ePollDataPtr->m_socketFd, buf, BUFSIZE, 0);

    if (nbytes <= 0) // Got error or connection closed by client
    {
        if (nbytes == 0) // Connection closed
            std::cout << "socket " << *client << " hung up\n";
        else
            std::cerr << "Error: recv() failed\n";
    }
    else // We got some good data from a client
    {
        std::cout << "nbytes = " << nbytes << '\n';
        HttpMessage message(buf);
        std::cout << "\n\nBUF:\n"
                  << buf << "\n###\n\n";
        client->requestMessage += message;
        if (!client->requestMessage.isComplete())
        {
            std::cout << "Message not complete...\n\n";
            return;
        }
        std::cout << "Mission completed!!!\n\n"
                  << std::endl;
        std::cout << "\n\n|" << client->requestMessage.getRawRequest() << "|\n\n";
        if (client->requestMessage.isValidHttpMessage())
        {
            std::cout << "HttpMessage is valid\n\n";
            HttpRequest httpRequest{client->requestMessage};
            sendResponse(createResponse(httpRequest), client->m_socketFd);
        }
    }
    close(ePollDataPtr->m_socketFd);
    delete client;
    return;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        throw std::runtime_error("usage: webserv [port]\n\n\n");

    MultiplexerIO multiplexerio{};

    for (int i{1}; i < argc; i++)
    {
        TcpServer *tcpserver = new TcpServer{argv[i]};
        multiplexerio.m_servers.push_back(tcpserver);
    }

    for (auto &server : multiplexerio.m_servers)
        multiplexerio.addSocketToEpollFd(server);

    while (true)
    {
        int epollCount{epoll_wait(multiplexerio.m_epollfd, multiplexerio.m_events.data(), MAX_EVENTS, (3 * 60 * 1000))};
        if (epollCount == -1)
        {
            std::perror("epoll_wait() failed");
            throw std::runtime_error("Error: epoll_wait() failed\n");
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
                    Client *clientsocket = new Client{server->m_socketFd};
                    multiplexerio.addSocketToEpollFd(clientsocket);
                    server->m_clientSockets.push_back(clientsocket);
                }
                else // If not the listener, we're just a regular client
                    do_use_fd(ePollDataPtr);
            }
        }
    }
}
