#include "Multiplexer.hpp"
#include "Response.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

// default constructor
Multiplexer::Multiplexer(void)
{
    m_epollfd = epoll_create(1);
    if (m_epollfd == -1)
        throw std::system_error(errno, std::generic_category(), "epoll_create()");

    if (addToEpoll(&signal, EPOLLIN, signal.m_socketFd))
        throw std::system_error(errno, std::generic_category(), "addToEpoll()");

    spdlog::debug("{} constructor", *this);
}

// destructor
Multiplexer::~Multiplexer(void)
{
    close(m_epollfd);

    spdlog::debug("{} destructor", *this);
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

void Multiplexer::removeFromEpoll(int fd)
{
    if (modifyEpollEvents(nullptr, 0, fd) == -1)
        throw StatusCodeException(500, "modifyEpollEvents()", errno);
    if (epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, NULL) == -1)
        throw StatusCodeException(500, "modifyEpollEvents()", errno);
}

void Multiplexer::removeClientBySocketFd(Client *clientWanted)
{
    auto it = std::find_if(m_clients.begin(), m_clients.end(), [clientWanted](Client *client)
                           { return client->m_socketFd == clientWanted->m_socketFd; });

    if (it != m_clients.end())
        m_clients.erase(it);
    else
        spdlog::error("Could not find client in m_clients");
    delete clientWanted;
}