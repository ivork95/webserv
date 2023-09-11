#ifndef HTTPMESSAGE_HPP
#define HTTPMESSAGE_HPP

#include <map>
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h> // must be included

class HttpMessage
{
public:
    std::string m_rawMessage{};
    std::map<std::string, std::string> m_requestHeaders{};
    int m_contentLength{};
    bool isContentLengthConverted{false};

    // member functions
    std::pair<std::string, std::string> parseFieldLine(const std::string &fieldLine, const std::string &keyDelim, size_t keyDelimPos) const;
    std::map<std::string, std::string> fieldLinesToHeaders(std::string &fieldLines) const;
    void requestHeadersPrint(const std::map<std::string, std::string> &headers) const;

    // getters/setters
    void setRequestHeaders(void);
    void setContentLength(void);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const HttpMessage &httpmessage);
};

#endif
