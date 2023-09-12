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
#include "HttpMessage.hpp"

class HttpRequest
{
public:
    std::string m_rawMessage{};
    std::map<std::string, std::string> m_requestHeaders{};
    int m_contentLength{};
    std::vector<std::string> m_methodPathVersion{};
    int m_client_max_body_size{999999};
    std::string m_body{};
    std::string m_boundaryCode{};
    std::string m_fileName{};

    // constructor
    HttpRequest(std::string rawMessage, std::map<std::string, std::string> requestHeaders, int m_contentLength);

    // getters/setters
    void setFileName(void);
    void setBoundaryCode(void);
    void setBody(void);

    // methods
    std::vector<std::string> split(const std::string &str);
    std::string getBoundaryCode(void);
    std::string getBody(const std::string &boundaryCode);
    std::string getGeneralHeaders(const std::string &boundaryCode);
    std::string getFileName(const std::string &boundaryCode);
    void requestHeadersPrint(const std::map<std::string, std::string> &headers) const;

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const HttpRequest &httprequest);
};

#endif