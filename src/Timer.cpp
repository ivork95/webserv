#include "Timer.hpp"

// constructor
Timer::Timer(Client &client) : m_client(client)
{
    m_spec.it_value.tv_sec = 180;
    m_spec.it_value.tv_nsec = 0;
    m_spec.it_interval.tv_sec = 0;
    m_spec.it_interval.tv_nsec = 0;

    m_socketFd = timerfd_create(CLOCK_REALTIME, O_NONBLOCK);
    if (m_socketFd == -1)
        throw std::system_error(errno, std::generic_category(), "timerfd_create()");
    if (timerfd_settime(m_socketFd, 0, &m_spec, NULL) == -1)
    {
        close(m_socketFd);
        throw std::system_error(errno, std::generic_category(), "timerfd_settime()");
    }

    spdlog::debug("{} constructor", *this);
}

Timer::~Timer(void)
{
    close(m_socketFd);
    spdlog::debug("{} destructor", *this);
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const Timer &timer)
{
    out << "Timer(" << timer.m_socketFd << ")";

    return out;
}