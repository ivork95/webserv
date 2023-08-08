#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#define BUFSIZE 1024

#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <memory>
#include "ClientSocket.hpp"

class TcpServer
{
public:
    int m_serverSocket{};
    struct sockaddr_in m_serverAddr
    {
    };
    std::vector<std::unique_ptr<ClientSocket>> m_clientSockets{};

    // default constructor
    TcpServer(void) = delete;

    // port constructor
    TcpServer(unsigned int port);

    // destructor
    ~TcpServer(void);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const TcpServer &tcpserver);
};

#endif
