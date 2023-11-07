#ifndef STATUS_CODES_HPP
#define STATUS_CODES_HPP

#include <stdexcept>
#include <exception>
#include <system_error>

class StatusCodeException : public std::system_error
{
private:
    int m_statusCode{};

public:
    StatusCodeException(int statusCode, const std::string &error, int ev = 0) : std::system_error(ev, std::generic_category(), error), m_statusCode(statusCode){};

    int getStatusCode(void) const { return m_statusCode; }
};

#endif