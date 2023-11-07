#include "Multiplexer.hpp"
#include "Response.hpp"

// default constructor
Multiplexer::Multiplexer(void)
{
    m_epollfd = epoll_create(1);
    if (m_epollfd == -1)
        throw std::system_error(errno, std::generic_category(), "epoll_create()");

    if (addToEpoll(&signal, EPOLLIN, signal.m_socketFd))
        throw std::system_error(errno, std::generic_category(), "addToEpoll()");

    std::cout << *this << " default constructor called\n";
}

// destructor
Multiplexer::~Multiplexer(void)
{
    std::cout << *this << " destructor called\n";

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

int Multiplexer::modifyEpollEvents(ASocket *ptr, int events, int fd)
{
    struct epoll_event ev
    {
    };
    ev.data.ptr = ptr;
    ev.events = events;

    return epoll_ctl(m_epollfd, EPOLL_CTL_MOD, fd, &ev);
}

int Multiplexer::addToEpoll(ASocket *ptr, int events, int fd)
{
    struct epoll_event ev
    {
    };
    ev.data.ptr = ptr;
    ev.events = events;

    return epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &ev);
}