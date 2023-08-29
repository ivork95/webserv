#include "../include/HttpResponse.hpp"

HttpResponse::HttpResponse(void)
{
	return ;
}

HttpResponse::HttpResponse(int statusCode, const std::string& reasonPhrase) : m_statusCode(statusCode), m_reasonPhrase(reasonPhrase)
{
	return ;
}

HttpResponse::~HttpResponse() 
{
	return ;
}

void HttpResponse::setHeader(const std::string& name, const std::string& value)
{
	m_headers[name] = value;
	return ;
}

void HttpResponse::setBody(const std::string& body)
{
	m_body = body;
	return ;
}

void HttpResponse::setStatus(int statusCode, std::string reasonPhrase)
{
    m_statusCode = statusCode;
    m_reasonPhrase = reasonPhrase;
	return ;
}

void HttpResponse::setHtmlContent(std::ifstream &file)
{
    std::string htmlContent;
    char character;

    while (file.get(character)) {
        htmlContent += character;
    }
    setHeader("Content-Type", "text/html");
    setBody(htmlContent);
}

int HttpResponse::getStatusCode(void)
{
    return m_statusCode;
}

std::string HttpResponse::generateResponse() const 
{
	std::ostringstream response;

	response << "HTTP/1.1 " << m_statusCode << " " << m_reasonPhrase << "\r\n";
	for (const auto& header : m_headers)
		response << header.first << ": " << header.second << "\r\n";
	response << "Content-Length: " << m_body.length() << "\r\n";
	response << "\r\n" << m_body;
	return response.str();
}
