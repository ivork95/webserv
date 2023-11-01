#include "Socket.hpp"

// destructor
Socket::~Socket(void)
{
    // spdlog::debug("Socket destructor called");
	Logger::getInstance().debug("Socket destructor called");
}
