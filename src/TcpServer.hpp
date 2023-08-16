#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <memory>
#include "ClientSocket.hpp"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

class TcpServer
{
public:
    int m_serverSocket{};
    struct addrinfo m_hints
    {
    };
    std::vector<std::unique_ptr<ClientSocket>> m_clientSockets{};

    // default constructor
    TcpServer(void) = delete;

    // port constructor
    TcpServer(const char *port);

    // destructor
    ~TcpServer(void);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const TcpServer &tcpserver);
};

#endif
