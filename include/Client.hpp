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

#include "Server.hpp"
#include "Timer.hpp"
#include "Socket.hpp"
#include "Request.hpp"
#include "Message.hpp"
#include "Multiplexer.hpp"

#define BUFSIZE 256

class Server;
class Timer;

class Client : public Socket
{
public:
    const Server &m_server;
    Request m_request{};
    Timer *m_timer{}; // Change to smart ptr
    struct sockaddr_storage m_remoteaddr
    {
    };
    socklen_t m_addrlen{sizeof(m_remoteaddr)};

    // default constructor
    Client(void) = delete;

    // server constructor
    Client(const Server &server);

    // destructor
    ~Client(void);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const Client &client);

    // member functions
    void handleConnectedClient(std::vector<Socket *> &toBeDeleted);
};

#endif