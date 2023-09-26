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

// #define BUFSIZE 256



// void handleConnectedClient(Client *client)
// {
//     char buf[BUFSIZE + 1]{}; // Buffer for client data
//     int nbytes = recv(client->m_socketFd, buf, BUFSIZE, 0);
//     if (nbytes <= 0)
//     {
//         if (nbytes == 0) // Connection closed
//             spdlog::info("socket {} hung up", *client);
//         else if (nbytes < 0) // Got error or connection closed by client
//             spdlog::critical("Error: recv() failed");
//         delete client;
//         return;
//     }

//     try
//     {
//         // We got some good data from a client
//         spdlog::info("nbytes = {}", nbytes);

//         client->httpRequest.m_rawMessage.append(buf, buf + nbytes);

//         size_t fieldLinesEndPos = client->httpRequest.m_rawMessage.find("\r\n\r\n");
//         if (fieldLinesEndPos == std::string::npos)
//         {
//             spdlog::warn("message incomplete [...]");
//             return;
//         }

//         if (client->httpRequest.m_requestHeaders.empty())
//             client->httpRequest.setRequestHeaders();

//         if (client->httpRequest.m_requestHeaders.contains("Content-Length"))
//         {
//             if (!client->httpRequest.m_isContentLengthConverted)
//                 client->httpRequest.setContentLength();
//             if (client->httpRequest.m_contentLength > static_cast<int>((client->httpRequest.m_rawMessage.length() - (fieldLinesEndPos + 4))))
//             {
//                 spdlog::warn("Content-Length not reached [...]");
//                 return;
//             }
//             spdlog::info("Content-Length reached!");
//         }
//         spdlog::info("message complete!");
//         spdlog::info("client->httpRequest.m_rawMessage = \n|{}|", client->httpRequest.m_rawMessage);

//         if (timerfd_settime(client->timer->m_socketFd, 0, &client->timer->m_spec, NULL) == -1)
//             throw StatusCodeException(500, "Error: timerfd_settime()");

//         client->httpRequest.setMethodPathVersion();
//         if (client->httpRequest.m_methodPathVersion.size() != 3)
//             throw StatusCodeException(400, "Error: not 3 elements in request-line");
//         if (client->httpRequest.m_methodPathVersion[2] != "HTTP/1.1")
//             throw StatusCodeException(505, "Warning: http version not allowed");
//         if ((client->httpRequest.m_methodPathVersion[0] != "GET") && (client->httpRequest.m_methodPathVersion[0] != "POST") && (client->httpRequest.m_methodPathVersion[0] != "DELETE"))
//             throw StatusCodeException(405, "Warning: method not allowed");

//         if (client->httpRequest.m_methodPathVersion[0] == "POST")
//         {
//             if (client->httpRequest.m_contentLength > client->httpRequest.m_client_max_body_size)
//                 throw StatusCodeException(413, "Warning: contentLength larger than max_body_size");

//             client->httpRequest.setBoundaryCode();
//             client->httpRequest.setGeneralHeaders();
//             client->httpRequest.setFileName();
//             client->httpRequest.setBody();
//             client->httpRequest.bodyToDisk("./www/" + client->httpRequest.m_fileName);
//         }
//     }
//     catch (const StatusCodeException &e)
//     {
//         client->httpRequest.m_statusCode = e.getStatusCode();
//         spdlog::warn(e.what());
//     }
//     spdlog::critical(client->httpRequest);
//     client->isWriteReady = true;
// }

// void run(std::vector<TcpServer *> servers)
// {
//     // std::vector<TcpServer *> servers{};
//     // for (int i{1}; i < argc; i++)
//     //     servers.push_back(new TcpServer{argv[i]});

//     MultiplexerIO &multiplexerio = MultiplexerIO::getInstance();

//     for (auto &server : servers)
//         multiplexerio.addSocketToEpollFd(server, EPOLLIN);

//     while (true)
//     {
//         int epollCount{epoll_wait(multiplexerio.m_epollfd, multiplexerio.m_events.data(), MAX_EVENTS, -1)};
//         if (epollCount < 0)
//         {
//             std::perror("epoll_wait() failed");
//             throw std::runtime_error("Error: epoll_wait() failed\n");
//         }

//         for (int i{0}; i < epollCount; i++) // Run through the existing connections looking for data to read
//         {
//             Socket *ePollDataPtr{static_cast<Socket *>(multiplexerio.m_events.at(i).data.ptr)};

//             if ((multiplexerio.m_events.at(i).events & EPOLLIN) && ePollDataPtr->isReadReady) // If someone's ready to read
//             {
//                 if (TcpServer *server = dynamic_cast<TcpServer *>(ePollDataPtr)) // If listener is ready to read, handle new connection
//                 {
//                     Client *client = new Client{*server};
//                     multiplexerio.addSocketToEpollFd(client, EPOLLIN | EPOLLOUT | EPOLLRDHUP);
//                     multiplexerio.addSocketToEpollFd(client->timer, EPOLLIN);
//                 }
//                 else if (Timer *timer = dynamic_cast<Timer *>(ePollDataPtr))
//                 {
//                     spdlog::warn("Timeout expired. Closing: {}", *(timer->m_client));
//                     delete timer->m_client;
//                 }
//                 else if (Client *client = dynamic_cast<Client *>(ePollDataPtr)) // If not the listener, we're just a regular client
//                 {
//                     handleConnectedClient(client);
//                 }
//             }
//             else if ((multiplexerio.m_events.at(i).events & EPOLLOUT) && ePollDataPtr->isWriteReady) // If someone's ready to write
//             {
//                 if (Client *client = dynamic_cast<Client *>(ePollDataPtr))
//                 {
//                     HttpResponse response{client->httpRequest};
//                     response.responseHandle();
//                     spdlog::critical(response);

//                     std::string s{response.responseBuild()};
//                     spdlog::critical("s = |{}|", s);
//                     send(client->m_socketFd, s.data(), s.length(), 0);
//                     delete (client);
//                 }
//             }
//         }
//     }
// }

// #define PARSTER true // parster

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

	// // To run only the parser and display the output
	// #if (PARSTER)
	// 	std::cout << "\n\t\t -----------------\n \t\t[  SERVER CONFIG  ]\n\t\t -----------------\n";
	// 	for (size_t i = 0; i < config.serversConfig.size(); ++i) {
	// 		std::cout << config.serversConfig[i];
	// 	}
	// 	return 0;
	// #endif

    spdlog::set_level(spdlog::level::debug);

    std::vector<TcpServer *> servers{};
    for (auto &serverConfig : config.serversConfig)
    {
        spdlog::critical("YOO");
        servers.push_back(new TcpServer{serverConfig.getPortNb().c_str()});
    }

	// for (size_t i = 0; i < config.serversConfig.size(); i++) {
    //     servers.push_back(new TcpServer{config.serversConfig.at(i).getPortNb().c_str()});
    //     spdlog::debug("Create new tcp server");
    // }
    MultiplexerIO &multiplexerio = MultiplexerIO::getInstance();
    multiplexerio.run(servers);
    return 0;
}
