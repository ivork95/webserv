#include "../include/HttpMessage.hpp"

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

HttpMessage::HttpMessage(const HttpMessage &src)
{
    m_rawRequest = src.getRawRequest();
    m_method = src.getMethod();
    m_version = src.getVersion();
    m_path = src.getPath();
    return ;
}

HttpMessage::~HttpMessage( void )
{
    return ;
}

HttpMessage& HttpMessage::operator+=(const HttpMessage& src)
{
    this->m_rawRequest += src.getRawRequest();
    return *this;
}

const std::string HttpMessage::getMethod(void) const
{
    return m_method;
}

const std::string HttpMessage::getVersion(void) const
{
    return m_version;
}

const std::string HttpMessage::getPath(void) const
{
    return m_path;
}

const std::string HttpMessage::getRawRequest(void) const
{
    return m_rawRequest;
}

bool HttpMessage::isComplete(void)
{
    size_t contentLengthValue;
    size_t breakLinePos = m_rawRequest.find("\r\n\r\n");
    size_t contentLengthPos = m_rawRequest.find("Content-Length:");
    if (breakLinePos == std::string::npos)
        return false;
    else if (contentLengthPos != std::string::npos)
    {
        contentLengthPos += 16;
        size_t contentLengthEnd = m_rawRequest.find("\r\n", contentLengthPos);
        contentLengthValue = atoi(m_rawRequest.substr(contentLengthPos, contentLengthEnd - contentLengthPos).c_str());
        if (contentLengthValue == 0)
        {
            std::cerr << "ERROR retreiving Content-Length value\n";
            return false;
        }
        if (contentLengthValue > m_rawRequest.length() - (breakLinePos + 4))
            return false;
    }
    setStartLineValues();
    return true;
}

void HttpMessage::setStartLineValues(void)
{
	std::istringstream stream(m_rawRequest);

	std::string startLine;
	std::getline(stream, startLine);

	std::istringstream startLineStream(startLine);
	startLineStream >> m_method >> m_path >> m_version;
}


bool HttpMessage::isValidHttpMessage(void) const
{
    if (!m_version.compare("HTTP/1.1"))
        return true;
    return false;
}
