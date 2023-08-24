#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <memory>
#include "Client.hpp"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "Socket.hpp"

class TcpServer : public Socket
{
public:
    struct addrinfo m_hints
    {
    };
    std::vector<Client *> m_clientSockets{};

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
