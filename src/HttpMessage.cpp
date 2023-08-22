#include "HttpMessage.hpp"

// constructor
HttpMessage::HttpMessage( void )
{
    return ;
}

HttpMessage::HttpMessage(const std::string &rawRequest)
{
    m_rawRequest = rawRequest;
    return ;
}

//overload operator
HttpMessage& HttpMessage::operator+=(const HttpMessage& src)
{
    this->m_rawRequest += src.getRawRequest();
    return *this;
}


// member function
std::string HttpMessage::getRawRequest(void) const
{
    return m_rawRequest;
}

bool HttpMessage::isComplete(void) const
{
    if (m_rawRequest.find("\r\n\r\n") == std::string::npos)
        return false;
    return true;
}


bool HttpMessage::isValidHttpMessage(void) const
{
    if (!m_version.compare("HTTP/1.1"))
        return true;
    return false;
}
