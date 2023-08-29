#ifndef MULTIPLEXERIO_HPP
#define MULTIPLEXERIO_HPP

#include <array>
#include <sys/epoll.h>
#include "../includes/TcpServer.hpp"

#define MAX_EVENTS 10 // The maximum number of events to be returned from epoll_wait()

class MultiplexerIO
{
public:
    int m_epollfd{};
    std::vector<TcpServer *> m_servers{};
    std::array<struct epoll_event, MAX_EVENTS> m_events{};

    // default constructor
    MultiplexerIO(void);

    // destructor
    ~MultiplexerIO(void);

    // member functions
    void addSocketToEpollFd(Socket *ptr);
    void deleteSocketFromEpollFd(int socket);
};

#endif