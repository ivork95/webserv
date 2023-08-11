#include "ServerIO.hpp"

// default constructor
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

// destructor
ServerIO::~ServerIO(void)
{
    std::cout << "ServerIO destructor called\n";

    close(m_epollFD);
}

// member functions
void ServerIO::addSocketToEpollFd(int socket)
{
    struct epoll_event event
    {
    };

    event.events = EPOLLIN | EPOLLOUT;
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
