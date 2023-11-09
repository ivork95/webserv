#ifndef HTTPMESSAGE_HPP
#define HTTPMESSAGE_HPP

#include <map>
#include <string>
#include <iostream>

#include "StatusCodeException.hpp"

class Message
{
public:
    std::string m_rawMessage{};
    std::map<std::string, std::string> m_requestHeaders{};
    int m_contentLength{};
    bool m_isContentLengthConverted{false};

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const Message &message);

    // getters/setters
    void requestHeadersSet(void);
    void contentLengthSet(void);

    // methods
    std::pair<std::string, std::string> fieldLineParse(const std::string &fieldLine, const std::string &keyDelim, size_t keyDelimPos) const;
    std::map<std::string, std::string> fieldLinesToHeaders(std::string &fieldLines);
    void requestHeadersPrint(const std::map<std::string, std::string> &headers) const;
};

#endif