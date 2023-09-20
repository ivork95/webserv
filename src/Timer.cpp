#include "Timer.hpp"

// client constructor
Timer::Timer(Client *client) : m_client(client)
{
    m_spec.it_value.tv_sec = 180;
    m_spec.it_value.tv_nsec = 0;
    m_spec.it_interval.tv_sec = 0;
    m_spec.it_interval.tv_nsec = 0;

    m_socketFd = timerfd_create(CLOCK_REALTIME, O_NONBLOCK);
    if (m_socketFd == -1)
    {
        std::perror("timerfd_create() failed");
        throw std::runtime_error("Error: timerfd_create() failed\n");
    }

    if (timerfd_settime(m_socketFd, 0, &m_spec, NULL) == -1)
    {
        std::perror("timerfd_settime failed");
        throw std::runtime_error("Error: timerfd_settime failed\n");
    }

    spdlog::debug("{} constructor called", *this);
}

// destructor
Timer::~Timer(void)
{
    spdlog::debug("{} destructor called", *this);

    close(m_socketFd);
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const Timer &timer)
{
    out << "Timer(" << timer.m_socketFd << ")";

    return out;
}