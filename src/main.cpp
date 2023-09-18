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

std::string generateHtmlString(const std::string& path)
{
    std::stringstream htmlStream;
    std::string folderPath = "." + path;
    
    htmlStream << "<html><head><title>File and Directory List</title></head><body>\n";
    htmlStream << "<h1>Files and Directories in " << folderPath << "</h1>\n";
    htmlStream << "<ul>\n";

    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        const std::string entryName = entry.path().filename().string();
        if (std::filesystem::is_directory(entry)) {
            htmlStream << "<li><strong>Directory:</strong> <a href=\"" << path + "/" + entryName << "\">" << entryName << "</a></li>\n";
        } else if (std::filesystem::is_regular_file(entry)) {
            htmlStream << "<li><strong>File:</strong> " << entryName << " ";
            htmlStream << "<button onclick=\"deleteFile('" << path + "/" + entryName << "')\">Delete</button></li>\n";
        }
    }

    htmlStream << "</ul>\n";
    htmlStream << "<script>\n";
    htmlStream << "function deleteFile(fileName) {\n";
    htmlStream << "    if (confirm('Are you sure you want to delete ' + fileName + '?')) {\n";
    htmlStream << "        var xhr = new XMLHttpRequest();\n";
    htmlStream << "        xhr.open('DELETE', fileName, true);\n";
    htmlStream << "        xhr.onreadystatechange = function() {\n";
    htmlStream << "            if (xhr.readyState === 4 && xhr.status === 200) {\n";
    htmlStream << "                alert('File ' + fileName + ' deleted.');\n";
    htmlStream << "                // Refresh the page or update the file list as needed\n";
    htmlStream << "            }\n";
    htmlStream << "        };\n";
    htmlStream << "        xhr.send();\n";
    htmlStream << "    }\n";
    htmlStream << "}\n";
    htmlStream << "</script>\n";
    htmlStream << "</body></html>\n";

    return htmlStream.str();
}

bool isDirectory(std::string path)
{
    if (std::filesystem::is_directory(path))
        return  true;
    return false;
}

HttpResponse handleRequest (Client *client)
{
    HttpResponse httpresponse{};

    if (!client->httpRequest.m_methodPathVersion[0].compare("GET"))
    {
        spdlog::debug("GET method");

        std::string path{client->httpRequest.m_methodPathVersion[1]};
        if (!path.compare("/"))
        {
            path = "./www/index.html";
            httpresponse.setBody(path);
        }
        else if (path.find("/cgi-bin/") != std::string::npos)
        {
            CGI CGI(path);
            // what does child process do when error thrown?
            CGI.execute();
            httpresponse.m_body = std::string(CGI.m_readBuf);
        }
        else if (isDirectory("." + path))
        {   
            spdlog::debug("PATH IS DIR");
            httpresponse.m_body = generateHtmlString(path);
        }
        else
        {
            path = "./www" + client->httpRequest.m_methodPathVersion[1] + ".html";
            httpresponse.setBody(path);
        }
    }
    else if (!client->httpRequest.m_methodPathVersion[0].compare("POST"))
    {
        spdlog::info("POST method");

        if (client->httpRequest.m_contentLength > client->httpRequest.m_client_max_body_size)
            throw HttpStatusCodeException(413);
        // if (client->httpRequest.m_contentLength > client->httpRequest.m_client_max_body_size)
        //     httpresponse.m_statusLine = "HTTP/1.1 413 Payload Too Large";
        else if (client->httpRequest.m_methodPathVersion[1].find("/cgi-bin/") != std::string::npos)
        {
            CGI CGI(client->httpRequest.m_methodPathVersion[1], "num1=5&num2=5");
            // set correct body for cgi parameters
            CGI.execute();
            httpresponse.m_body = std::string(CGI.m_readBuf);
        }
        else
        {
            client->httpRequest.setBoundaryCode();
            client->httpRequest.setGeneralHeaders();
            client->httpRequest.setFileName();
            client->httpRequest.setBody();
            client->httpRequest.bodyToDisk("./www/" + client->httpRequest.m_fileName);
        }
    }
    else if (!client->httpRequest.m_methodPathVersion[0].compare("DELETE"))
    {
        spdlog::debug("DELETE method");
        std::string path{client->httpRequest.m_methodPathVersion[1]};
        std::string allowedPath = "/www/files";

        if (path.compare(0, allowedPath.length(), allowedPath) != 0)
            throw HttpStatusCodeException(410);
        else if (std::remove("./www/hello.txt") == 0)
        {
            httpresponse.m_body = "Succes";
        }
        else
        {
            throw HttpStatusCodeException(411);
        }
    }
    else
        throw HttpStatusCodeException(501);
    spdlog::info(client->httpRequest);
    return (httpresponse);
}

void handleConnectedClient(Client *client)
{
    char buf[BUFSIZE + 1]{}; // Buffer for client data
    int nbytes = recv(client->m_socketFd, buf, BUFSIZE, 0);

    if (timerfd_settime(client->timer->m_socketFd, 0, &client->timer->m_spec, NULL) == -1)
        throw std::runtime_error("Error: timerfd_settime()\n");

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
                client->httpRequest.setContentLength();
            if (client->httpRequest.m_contentLength > static_cast<int>((client->httpRequest.m_rawMessage.length() - (fieldLinesEndPos + 4))))
            {
                spdlog::warn("Content-Length not reached [...]");
                return;
            }
            spdlog::info("Content-Length reached!");
        }

        spdlog::info("message complete!");
        spdlog::info("m_rawMessage = \n|{}|", client->httpRequest.m_rawMessage);

        client->httpRequest.setMethodPathVersion();

        // HttpResponse httpresponse{};
        /*
        The HTTP version used in the request is not supported by the server. 505 HTTP Version Not Supported
        */

        /*
        A server MUST respond with a 400 (Bad Request) status code to any HTTP/1.1 request message that lacks a Host header field and to any request message that contains more than one Host header field line or a Host header field with an invalid field value.
        */
        HttpResponse response{};
        try
        {
            response = handleRequest(client);
        }
        catch (const HttpStatusCodeException &e)
        {
            spdlog::debug(e.what());
            response = HttpResponse(e.getErrorCode());
        }
        std::string s{response.responseBuild()};
        send(client->m_socketFd, s.data(), s.length(), 0);
        delete (client);
        return ;
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
