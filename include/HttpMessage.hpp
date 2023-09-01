#ifndef HTTPMESSAGE_HPP
#define HTTPMESSAGE_HPP

#include <string>
#include <sstream>
#include <iostream>

class HttpMessage
{
public:
    HttpMessage(void);
    HttpMessage(const std::string &rawRequest);
    HttpMessage(const HttpMessage &src);
    ~HttpMessage(void);

    HttpMessage &operator+=(const HttpMessage &src);

    const std::string getMethod(void) const;
    const std::string getPath(void) const;
    const std::string getVersion(void) const;
    const std::string getRawRequest(void) const;

    bool isValidHttpMessage(void) const;
    bool isComplete(void);
    void setStartLineValues(void);

private:
    std::string m_rawRequest{};
    std::string m_method{};
    std::string m_path{};
    std::string m_version{};
};

#endif
