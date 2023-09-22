#ifndef HTTPMESSAGE_HPP
#define HTTPMESSAGE_HPP

#include <map>
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include "StatusCodes.hpp"

class HttpMessage
{
public:
    std::string m_rawMessage{};
    std::map<std::string, std::string> m_requestHeaders{};
    int m_contentLength{};
    bool m_isContentLengthConverted{false};

    // constructor
    HttpMessage(void);

    // copy constructor

    // copy assignment operator overload

    // destructor
    ~HttpMessage(void);

    // getters/setters
    void setRequestHeaders(void);
    void setContentLength(void);

    // methods
    std::pair<std::string, std::string> parseFieldLine(const std::string &fieldLine, const std::string &keyDelim, size_t keyDelimPos) const;
    std::map<std::string, std::string> fieldLinesToHeaders(std::string &fieldLines);
    void requestHeadersPrint(const std::map<std::string, std::string> &headers) const;

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const HttpMessage &httpmessage);
};

#endif
