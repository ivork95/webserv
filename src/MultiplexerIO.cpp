#include "MultiplexerIO.hpp"
#include "HttpResponse.hpp"

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

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const MultiplexerIO &multiplexerio)
{
    out << "MultiplexerIO(" << multiplexerio.m_epollfd << ")";

    return out;
}

// member functions
MultiplexerIO &MultiplexerIO::getInstance(void)
{
    static MultiplexerIO instance;
    return instance;
}

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