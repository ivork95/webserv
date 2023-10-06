#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <memory>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <cstring>

#include "Client.hpp"
#include "Socket.hpp"
#include "ServerConfig.hpp"

class TcpServer : public Socket
{
public:
    struct addrinfo m_hints
    {
    };

    const ServerConfig &m_serverconfig;

    // default constructor
    TcpServer(void) = delete;

    // serverConfig constructor
    TcpServer(const ServerConfig &serverconfig);

    // copy constructor
    TcpServer(const TcpServer &source) = delete;

    // copy assignment operator overload
    TcpServer &operator=(const TcpServer &source) = delete;

    // destructor
    ~TcpServer(void);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const TcpServer &tcpserver);
};

#endif