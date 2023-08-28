#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>


class HttpResponse
{
	public:

		HttpResponse(void);
		HttpResponse(int statusCode, const std::string& reasonPhrase);
		~HttpResponse(void);

		void setHeader(const std::string& name, const std::string& value);
		void setBody(const std::string& body);
		void setStatus(int statusCode, std::string reasonPhrase);
        void setHtmlContent(std::ifstream &file);
        int  getStatusCode(void);


		std::string generateResponse() const;

	private:

		int			m_statusCode{};
		std::string m_reasonPhrase{};
		std::string m_body{};
		std::string m_htmlContent{};
		std::map<std::string, std::string> m_headers{};
};

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

#endif
