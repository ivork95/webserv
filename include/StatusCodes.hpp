#ifndef STATUS_CODES_HPP
#define STATUS_CODES_HPP

#include <exception>
#include <map>

class StatusCodeException : public std::runtime_error
{
private:
    int m_statusCode{};

public:
    StatusCodeException(int statusCode, const std::string &error) : std::runtime_error(error.c_str()){}; // std::runtime_error will handle the string
    // no need to override what() since we can just use std::runtime_error::what()

    int getStatusCode(void) const {return m_statusCode};
};

class HttpStatusCodeException : public std::exception
{
private:
    int errorCode;

public:
    HttpStatusCodeException(int errorCode) : errorCode(errorCode) {}

    const char *what() const noexcept override
    {
        return "HTTP Status Code Exception";
    }

    int getErrorCode() const
    {
        return errorCode;
    }
};

#endif