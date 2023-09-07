#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h> // must be included
#include <map>
#include <string>
#include <sstream>

class HttpRequest
{
public:
    size_t m_fieldLinesEndPos{};
    std::string m_rawMessage{};
    std::string m_method{};
    std::string m_target{};
    std::string m_version{};
    std::map<std::string, std::string> m_headers{};
    bool isContentLengthConverted{false};
    int m_contentLength{};
    int m_client_max_body_size{999999};

    // default constructor
    HttpRequest(void);

    // methods
    std::string requestLineParse(const std::string &rawMessage);
    void setMethodPathVersion(void);
    void setHeaders(void);
    void setContentLength(void);
    std::pair<std::string, std::string> parseFieldLine(const std::string &fieldLine, const std::string &keyDelim, size_t keyDelimPos) const;
    std::map<std::string, std::string> fieldLinesToHeaders(std::string &fieldLines) const;
    std::string postRequestHandle(void);
    void headersPrint(const std::map<std::string, std::string> &headers);
    std::string getBody(const std::string &boundaryCode);
    std::string getGeneralHeaders(const std::string &boundaryCode);
    std::string getBoundaryCode(void);
};

#endif