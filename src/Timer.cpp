#include "Timer.hpp"

Timer::Timer(Client *client) : m_client(client)
{
    // Set initial expiration to 2 seconds and interval to 1 second.
    m_spec.it_value.tv_sec = 5;
    m_spec.it_value.tv_nsec = 0;
    m_spec.it_interval.tv_sec = 0;
    m_spec.it_interval.tv_nsec = 0;

    // Create a new timer object.
    m_socketFd = timerfd_create(CLOCK_REALTIME, 0);
    if (m_socketFd == -1)
    {
        perror("timerfd_create");
        exit(EXIT_FAILURE);
    }

    // Set the timer.
    if (timerfd_settime(m_socketFd, 0, &m_spec, NULL) == -1)
    {
        perror("timerfd_settime");
        exit(EXIT_FAILURE);
    }

    spdlog::info("Timer default constructor called");
}

Timer::~Timer(void)
{
    spdlog::info("Timer destructor called");

    close(m_socketFd);
}