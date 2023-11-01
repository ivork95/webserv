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
    // spdlog::debug("{} default constructor called", *this);
	Logger::getInstance().debug("Multiplexer(" + std::to_string(m_epollfd) + ")" + " default constructor called");
}

// destructor
Multiplexer::~Multiplexer(void)
{
    // spdlog::debug("{} destructor called", *this);
	Logger::getInstance().debug("Multiplexer(" + std::to_string(m_epollfd) + ")" + " destructor called");

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

int Multiplexer::modifyEpollEvents(Socket *ptr, int events, int fd)
{
    struct epoll_event ev
    {
    };
    ev.data.ptr = ptr;
    ev.events = events;

    return epoll_ctl(m_epollfd, EPOLL_CTL_MOD, fd, &ev);
}

int Multiplexer::addToEpoll(Socket *ptr, int events, int fd)
{
    struct epoll_event ev
    {
    };
    ev.data.ptr = ptr;
    ev.events = events;

    return epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &ev);
}
