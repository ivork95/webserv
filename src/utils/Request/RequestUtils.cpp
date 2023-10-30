
#include "Request.hpp"
#include "Multiplexer.hpp"
#include "CGIPipeIn.hpp"
#include "CGIPipeOut.hpp"

void Request::updatedLocationConfigSet(const std::string &methodPath)
{
	// spdlog::warn("methodPath = {}", methodPath); // ? debug

    bool isLocationFound{false};
    for (const auto &location : m_client.m_server.m_serverconfig.getLocationsConfig())
    {
        if (methodPath == location.getRequestURI())
        {
            m_locationconfig = location;
            isLocationFound = true;
            break;
        }
    }
    if (!isLocationFound) // there's no matching URI
        throw StatusCodeException(404, "Error: no matching location/path found");
}
