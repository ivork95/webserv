#ifndef SERVERIO_HPP
#define SERVERIO_HPP

#include "iostream"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <array>
#include <cstring>

#define MAX_EVENTS 10 // The maximum number of events to be returned from epoll_wait()
#define TIMEOUT 60000 // Timeout for epoll_wait()

class ServerIO
{
public:
    int m_fd{};
    std::array<struct epoll_event, MAX_EVENTS> m_events{};
    struct epoll_event m_events2[MAX_EVENTS]{};

    // default constructor
    ServerIO(void);

    // member functions
    void addSocketToEpollFd(int m_serverSocket);
};

ServerIO::ServerIO(void)
{
    std::cout << "ServerIO default constructor called\n";

    m_fd = epoll_create(1);
    if (m_fd == -1)
    {
        std::perror("epoll_create() failed");
        throw std::runtime_error("Error: epoll_create() failed\n");
    }
}

void ServerIO::addSocketToEpollFd(int m_serverSocket)
{
    struct epoll_event event;

    event.events = EPOLLIN;
    event.data.fd = m_serverSocket;
    if (epoll_ctl(m_fd, EPOLL_CTL_ADD, m_serverSocket, &event) == -1)
    {
        std::perror("epoll_ctl() failed");
        throw std::runtime_error("Error: epoll_ctl() failed\n");
    }
}

#endif
