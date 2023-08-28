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

#endif
