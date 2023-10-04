#include "Socket.hpp"

// destructor
Socket::~Socket(void)
{
    spdlog::debug("Socket destructor called");
}

// member functions
int Socket::setNonBlocking(void)
{
    int result{};

    result = fcntl(m_socketFd, F_SETFL, O_NONBLOCK);
    if (result == -1)
    {
        std::perror("fcntl() failed");
        throw std::runtime_error("Error: fcntl() failed\n");
    }

    return result;
}
