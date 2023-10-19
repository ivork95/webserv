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
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <cstring>

#include "Client.hpp"
#include "Socket.hpp"
#include "ServerConfig.hpp"

class Server : public Socket
{
public:
    struct addrinfo m_hints
    {
    };

    const ServerConfig &m_serverconfig;

    // default constructor
    Server(void) = delete;

    // serverConfig constructor
    Server(const ServerConfig &serverconfig);

    // copy constructor
    Server(const Server &source) = delete;

    // copy assignment operator overload
    Server &operator=(const Server &source) = delete;

    // destructor
    ~Server(void);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const Server &server);
};

#endif