#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP

# include <map>
# include <string>
# include <sstream>
# include <iostream>
# include "HttpMessage.hpp"

# define GET "GET"
# define POST "POST"
# define DELETE "DELETE"

class HttpRequest : public HttpMessage
{
	public:

		HttpRequest();
		HttpRequest(const HttpMessage &message);
		~HttpRequest(void);

		const std::string &getBody(void) const;
		const std::string getFilePath(void) const;
		const std::map<std::string, std::string>& getHeaders(void) const;

	private:

		std::string m_body{};
		std::map<std::string, std::string> 	m_headers{};
};

std::ostream &operator<<(std::ostream &os, const HttpRequest &b);

#endif
