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
    std::vector<Client *> m_clients{};
    int m_epollfd{};
    std::array<struct epoll_event, MAX_EVENTS> m_events{};
    std::vector<Server *> m_servers{};
    bool isRunning{true};
    Signal signal;

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
    int removeFromEpoll(int fd);
    void removeClientBySocketFd(int socketFd);
};

#endif