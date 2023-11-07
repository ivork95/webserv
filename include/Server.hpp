#ifndef SERVER_HPP
#define SERVER_HPP

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
#include <cstring>

#include "Client.hpp"
#include "ASocket.hpp"
#include "ServerConfig.hpp"
#include "Logger.hpp"

#define BACKLOG 150 // how many pending connections queue will hold

class Server : public ASocket
{
public:
    struct addrinfo m_hints
    {
    };

    const ServerConfig &m_serverconfig;

    // default constructor
    Server(void) = delete;

    // constructor
    Server(const ServerConfig &serverconfig);

    // copy constructor
    Server(const Server &source) = delete;

    // copy assignment operator overload
    Server &operator=(const Server &source) = delete;

    // destructor
    ~Server(void);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const Server &server);

    void handleNewConnection(void) const;
};

#endif