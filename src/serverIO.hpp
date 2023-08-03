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
    int m_epollFD{};
    std::array<struct epoll_event, MAX_EVENTS> m_events{};

    // default constructor
    ServerIO(void);

    // destructor
    ~ServerIO(void);

    // member functions
    void addSocketToEpollFd(int socket);
    void deleteSocketFromEpollFd(int socket);
};

ServerIO::ServerIO(void)
{
    std::cout << "ServerIO default constructor called\n";

    m_epollFD = epoll_create(1);
    if (m_epollFD == -1)
    {
        std::perror("epoll_create() failed");
        throw std::runtime_error("Error: epoll_create() failed\n");
    }
}

ServerIO::~ServerIO(void)
{
    std::cout << "ServerIO destructor called\n";

    close(m_epollFD);
}

void ServerIO::addSocketToEpollFd(int socket)
{
    struct epoll_event event;

    event.events = EPOLLIN;
    event.data.fd = socket;
    if (epoll_ctl(m_epollFD, EPOLL_CTL_ADD, socket, &event) == -1)
    {
        std::perror("epoll_ctl() failed");
        throw std::runtime_error("Error: epoll_ctl() failed\n");
    }
}

void ServerIO::deleteSocketFromEpollFd(int socket)
{
    if (epoll_ctl(m_epollFD, EPOLL_CTL_DEL, socket, NULL) == -1)
    {
        std::perror("epoll_ctl() failed");
        throw std::runtime_error("Error: epoll_ctl() failed\n");
    }
    close(socket);
}

#endif
