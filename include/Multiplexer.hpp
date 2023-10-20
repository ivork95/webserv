#ifndef MULTIPLEXER_HPP
#define MULTIPLEXER_HPP

#include <sys/epoll.h>
#include <array>
#include <csignal>
#include <fstream>
#include <filesystem>
#include <sys/stat.h>

#include "Server.hpp"
#include "StatusCodes.hpp"
#include "Socket.hpp"

#define MAX_EVENTS 10 // The maximum number of events to be returned from epoll_wait()

class Multiplexer
{
private:
    // default constructor
    Multiplexer(void);

public:
    int m_epollfd{};
    std::array<struct epoll_event, MAX_EVENTS> m_events{};

    // copy constructor
    Multiplexer(const Multiplexer &) = delete;

    // copy assignment operator overload
    Multiplexer &operator=(const Multiplexer &) = delete;

    // destructor
    ~Multiplexer(void);

    // member functions
    static Multiplexer &getInstance(void);
    void modifyEpollEvents(Socket *ptr, int events);
    int addToEpoll(Socket *ptr, int events, int fd);

    // outstream operator overload
    friend std::ostream &
    operator<<(std::ostream &out, const Multiplexer &multiplexer);
};

#endif