#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h> // must be included
#include <map>

#include "TcpServer.hpp"
#include "Timer.hpp"
#include "Socket.hpp"
#include "HttpRequest.hpp"
#include "HttpMessage.hpp"

class TcpServer;
class Timer;

class Client : public Socket
{
public:
    Timer *timer{};
    const TcpServer &m_server;
    HttpRequest m_httprequest{};
    struct sockaddr_storage m_remoteaddr
    {
    };
    socklen_t m_addrlen{sizeof(m_remoteaddr)};

    // default constructor
    Client(void) = delete;

    // server constructor
    Client(const TcpServer &server);

    // destructor
    ~Client(void);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const Client &client);
};

#endif