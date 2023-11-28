#ifndef MULTIPLEXER_HPP
#define MULTIPLEXER_HPP

#include <sys/epoll.h>
#include <array>
#include <csignal>
#include <fstream>
#include <filesystem>
#include <sys/stat.h>
#include "Server.hpp"
#include "StatusCodeException.hpp"
#include "ASocket.hpp"
#include "Signal.hpp"

#define MAX_EVENTS 10 // The maximum number of events to be returned from epoll_wait()

class Server;

class Multiplexer
{
private:
    // default constructor
    Multiplexer(void);

public:
    Signal m_signal;
    std::vector<Server *> m_servers{};
    std::vector<Client *> m_clients{};
    std::array<struct epoll_event, MAX_EVENTS> m_events{};
    int m_epollfd{};
    bool m_isRunning{true};

    // copy constructor
    Multiplexer(const Multiplexer &source) = delete;

    // copy assignment operator overload
    Multiplexer &operator=(const Multiplexer &source) = delete;

    // destructor
    ~Multiplexer(void);

    // outstream operator overload
    friend std::ostream &
    operator<<(std::ostream &out, const Multiplexer &multiplexer);

    // member functions
    static Multiplexer &getInstance(void);
    int modifyEpollEvents(ASocket *ptr, int events, int fd);
    int addToEpoll(ASocket *ptr, int events, int fd);
    void removeFromEpoll(int fd);
    void removeClientBySocketFd(Client *client);
};

#endif