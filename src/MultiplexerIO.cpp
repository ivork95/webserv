#include "MultiplexerIO.hpp"

// default constructor
MultiplexerIO::MultiplexerIO(void)
{
    spdlog::info("MultiplexerIO default constructor called");

    m_epollfd = epoll_create(1);
    if (m_epollfd == -1)
    {
        std::perror("epoll_create() failed");
        throw std::runtime_error("Error: epoll_create() failed\n");
    }
}

// destructor
MultiplexerIO::~MultiplexerIO(void)
{
    spdlog::info("MultiplexerIO destructor called");

    close(m_epollfd);
}

// member functions
void MultiplexerIO::addSocketToEpollFd(Socket *ptr)
{
    struct epoll_event ev
    {
    };
    ev.data.ptr = ptr;
    ev.events = EPOLLIN | EPOLLOUT;

    if (epoll_ctl(m_epollfd, EPOLL_CTL_ADD, ptr->m_socketFd, &ev) == -1)
    {
        std::perror("epoll_ctl() failed");
        throw std::runtime_error("Error: epoll_ctl() failed\n");
    }
}
