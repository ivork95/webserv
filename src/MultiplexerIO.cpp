#include "MultiplexerIO.hpp"
#include "HttpResponse.hpp"

#define BUFSIZE 256

// default constructor
MultiplexerIO::MultiplexerIO(void)
{
    m_epollfd = epoll_create(1);
    if (m_epollfd == -1)
    {
        std::perror("epoll_create() failed");
        throw std::runtime_error("Error: epoll_create() failed\n");
    }
    spdlog::debug("{} default constructor called", *this);
}

// destructor
MultiplexerIO::~MultiplexerIO(void)
{
    spdlog::debug("{} destructor called", *this);

    close(m_epollfd);
}

// member functions
void MultiplexerIO::addSocketToEpollFd(Socket *ptr, int events)
{
    struct epoll_event ev
    {
    };
    ev.data.ptr = ptr;
    ev.events = events;

    if (epoll_ctl(m_epollfd, EPOLL_CTL_ADD, ptr->m_socketFd, &ev) == -1)
    {
        std::perror("epoll_ctl() failed");
        throw std::runtime_error("Error: epoll_ctl() failed\n");
    }
}

void MultiplexerIO::modifyEpollEvents(Socket *ptr, int events)
{
    struct epoll_event ev
    {
    };
    ev.data.ptr = ptr;
    ev.events = events;

    if (epoll_ctl(m_epollfd, EPOLL_CTL_MOD, ptr->m_socketFd, &ev) == -1)
        throw StatusCodeException(500, "Error: EPOLL_CTL_MOD failed");
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const MultiplexerIO &multiplexerio)
{
    out << "MultiplexerIO(" << multiplexerio.m_epollfd << ")";

    return out;
}