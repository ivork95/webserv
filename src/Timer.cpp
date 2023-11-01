#include "Timer.hpp"

// client constructor
Timer::Timer(Client &client) : m_client(client)
{
    m_spec.it_value.tv_sec = 180;
    m_spec.it_value.tv_nsec = 0;
    m_spec.it_interval.tv_sec = 0;
    m_spec.it_interval.tv_nsec = 0;

    m_socketFd = timerfd_create(CLOCK_REALTIME, O_NONBLOCK);
    if (m_socketFd == -1) {
        throw std::runtime_error("Error: timerfd_create() failed\n");
	}

    if (timerfd_settime(m_socketFd, 0, &m_spec, NULL) == -1) {
        throw std::runtime_error("Error: timerfd_settime failed\n");
	}

    // spdlog::debug("{} constructor called", *this);
	Logger::getInstance().debug("Timer(" + std::to_string(m_socketFd) + ") constructor called");
}

// destructor
Timer::~Timer(void)
{
    // spdlog::debug("{} destructor called", *this);
	Logger::getInstance().debug("Timer(" + std::to_string(m_socketFd) + ") destructor called");
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const Timer &timer)
{
    out << "Timer(" << timer.m_socketFd << ")";

    return out;
}

// std::string Timer::thisToString() const {

// 	std::ostringstream serverInfo;
// 	serverInfo << *this;
// 	std::string strThis = serverInfo.str();

// 	return strThis;
// }