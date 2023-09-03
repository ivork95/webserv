#include "Socket.hpp"

Socket::~Socket(void)
{
    std::cout << "Socket destructor called\n";
}

int Socket::setNonBlocking(void)
{
    int result{};
    int flags{};

    flags = ::fcntl(m_socketFd, F_GETFL, 0);

    if (flags == -1)
    {
        std::perror("fcntl() failed");
        throw std::runtime_error("Error: fcntl() failed\n");
    }

    flags |= O_NONBLOCK;

    result = fcntl(m_socketFd, F_SETFL, flags);
    if (result == -1)
    {
        std::perror("fcntl() failed");
        throw std::runtime_error("Error: fcntl() failed\n");
    }

    return result;
}
