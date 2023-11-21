#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <map>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include "Request.hpp"
#include "Timer.hpp"
#include "ASocket.hpp"
#include "Message.hpp"
#include "Multiplexer.hpp"

class Server;

class Client : public ASocket
{
private:
    const Server &m_server;
    Request m_request;
    Timer m_timer;

    struct sockaddr_storage m_remoteaddr
    {
    };
    socklen_t m_addrlen{sizeof(m_remoteaddr)};

public:
    // default constructor
    Client(void) = delete;

    // constructor
    Client(const Server &server);

    ~Client(void);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const Client &client);

    const Server &getServer(void) const { return m_server; }
    Request &getRequest(void) { return m_request; }
    Timer &getTimer(void) { return m_timer; }

    // member functions
    void handleConnectedClient();
};

#endif