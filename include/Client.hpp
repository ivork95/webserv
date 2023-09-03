#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "Socket.hpp"
#include "HttpMessage.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h> // must be included
#include "TcpServer.hpp"
#include <map>

class TcpServer;

class Client : public Socket
{
public:
    bool m_isStoiCalled{false};
    int m_contentLength{};
    std::string m_rawMessage{};
    std::string m_method{};
    std::string m_path{};
    std::string m_version{};
    int m_client_max_body_size{1000};
    std::map<std::string, std::string> m_headers{};
    std::string startLineParse(const std::string &rawMessage);
    void setMethodPathVersion(void);
    void setHeaders(void);
    void setContentLength(void);

    const TcpServer &m_server;
    struct sockaddr_storage m_remoteaddr
    {
    };
    socklen_t m_addrlen{sizeof(m_remoteaddr)};

    HttpMessage requestMessage;
    // default constructor
    Client(void) = delete;

    // serverSocket constructor
    Client(const TcpServer &server);

    // destructor
    ~Client(void);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const Client &clientsocket);
};

#endif
