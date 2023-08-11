#ifndef SERVERIO_HPP
#define SERVERIO_HPP

#include <array>
#include <sys/epoll.h>
#include "TcpServer.hpp"

#define MAX_EVENTS 10 // The maximum number of events to be returned from epoll_wait()

class ServerIO
{
public:
    int m_epollfd{};
    std::vector<std::unique_ptr<TcpServer>> m_servers{};
    std::array<struct epoll_event, MAX_EVENTS> m_events{};

    // default constructor
    ServerIO(void);

    // destructor
    ~ServerIO(void);

    // member functions
    void addSocketToEpollFd(int socket);
    void deleteSocketFromEpollFd(int socket);
};

#endif
