#include "Multiplexer.hpp"
#include "Response.hpp"

// default constructor
Multiplexer::Multiplexer(void)
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
Multiplexer::~Multiplexer(void)
{
    spdlog::debug("{} destructor called", *this);

    close(m_epollfd);
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const Multiplexer &multiplexer)
{
    out << "Multiplexer(" << multiplexer.m_epollfd << ")";

    return out;
}

// member functions
Multiplexer &Multiplexer::getInstance(void)
{
    static Multiplexer instance;
    return instance;
}

void Multiplexer::addSocketToEpollFd(Socket *ptr, int events)
{
    struct epoll_event ev
    {
    };
    ev.data.ptr = ptr;
    ev.events = events;

    if (epoll_ctl(m_epollfd, EPOLL_CTL_ADD, ptr->m_socketFd, &ev) == -1)
        throw std::runtime_error("Error: epoll_ctl() failed\n");
}

void Multiplexer::modifyEpollEvents(Socket *ptr, int events)
{
    struct epoll_event ev
    {
    };
    ev.data.ptr = ptr;
    ev.events = events;

    if (epoll_ctl(m_epollfd, EPOLL_CTL_MOD, ptr->m_socketFd, &ev) == -1)
        throw StatusCodeException(500, "Error: EPOLL_CTL_MOD failed");
}