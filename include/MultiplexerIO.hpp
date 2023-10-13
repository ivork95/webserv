#ifndef MULTIPLEXERIO_HPP
#define MULTIPLEXERIO_HPP

#include <sys/epoll.h>
#include <array>
#include <csignal>
#include <fstream>
#include <filesystem>
#include <sys/stat.h>
#include "TcpServer.hpp"
#include "StatusCodes.hpp"

#define MAX_EVENTS 10 // The maximum number of events to be returned from epoll_wait()

class MultiplexerIO
{
private:
    // default constructor
    MultiplexerIO(void);

public:
    int m_epollfd{};
    std::array<struct epoll_event, MAX_EVENTS> m_events{};

    // copy constructor
    MultiplexerIO(const MultiplexerIO &) = delete;

    // copy assignment operator overload
    MultiplexerIO &operator=(const MultiplexerIO &) = delete;

    // destructor
    ~MultiplexerIO(void);

    // member functions
    static MultiplexerIO &getInstance(void);
    void addSocketToEpollFd(Socket *ptr, int events);
    void modifyEpollEvents(Socket *ptr, int events);

    // outstream operator overload
    friend std::ostream &
    operator<<(std::ostream &out, const MultiplexerIO &multiplexerio);
};

#endif