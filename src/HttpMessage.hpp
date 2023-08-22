#ifndef HTTPMESSAGE_HPP
#define HTTPMESSAGE_HPP

#include <string>
#include <sstream>

class HttpMessage
{
public:

    HttpMessage(void);
    HttpMessage(const std::string& rawRequest);

    HttpMessage& operator+=(const HttpMessage &src);
    // HtpMessage(HttpMessage)
    // ~HttpMessage(void);

    // member function
    std::string getRawRequest(void) const;
    bool isComplete(void) const;
    bool isValidHttpMessage(void) const;

private:
    std::string m_rawRequest{};

    std::string m_version{};
};

#endif
