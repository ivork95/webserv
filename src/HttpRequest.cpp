#include "HttpRequest.hpp"

HttpRequest::HttpRequest(void)
{
    return ;
}

HttpRequest::HttpRequest(const HttpMessage &message) : HttpMessage(message)
{
	std::istringstream stream(getRawRequest());

	std::string startLine;
	std::getline(stream, startLine);

	std::istringstream startLineStream(startLine);

	std::string line;
	while (std::getline(stream, line) && !line.empty())
	{
		std::size_t colonPos = line.find(':');
		if (colonPos != std::string::npos)
        {
            std::string headerKey = line.substr(0, colonPos);
            std::string headerValue = line.substr(colonPos + 1);

            size_t lastNonSpace = headerValue.find_last_not_of(" \t\r\n");
            if (lastNonSpace != std::string::npos)
                headerValue = headerValue.substr(0, lastNonSpace + 1);
			m_headers[headerKey] = headerValue;
        }
	}
	while (std::getline(stream, line))
        m_body += line;
	return ;
}

HttpRequest::~HttpRequest(void)
{
	std::cout << "Destructor httpRequest" << std::endl;
	return ;
}

const std::string &HttpRequest::getBody(void) const
{
	return (m_body);
}

const std::map<std::string, std::string> &HttpRequest::getHeaders(void) const
{
	return (m_headers);
}

std::ostream& operator<<(std::ostream &os, const HttpRequest &b)
{
	os << "Method: " << b.getMethod() << std::endl; 
	os << "Path: " << b.getPath() << std::endl; 
	os << "Version: " << b.getVersion() << std::endl; 
	os << "Body: " << b.getBody() << std::endl;
	os << "headers: " << std::endl;
	std::map<std::string,std::string> headers = b.getHeaders();
	for (auto it = headers.begin(); it != headers.end(); it++)
		os << "\t{\"" << it->first << "\": \"" << it->second << "\"}" << std::endl;
	return (os);
}
