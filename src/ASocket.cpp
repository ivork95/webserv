#include "ASocket.hpp"

// destructor
ASocket::~ASocket(void)
{
    // spdlog::debug("ASocket destructor called");
    Logger::getInstance().debug("ASocket destructor called");
}
