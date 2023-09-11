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
    std::vector<std::string> m_methodPathVersion{};
    std::string m_rawMessage{};
    std::map<std::string, std::string> m_requestHeaders{};
    int m_contentLength{};
    int m_client_max_body_size{999999};

    // constructor
    HttpRequest(std::string rawMessage, std::map<std::string, std::string> requestHeaders, int m_contentLength);

    // methods
    std::vector<std::string> split(const std::string &str);

    std::string postRequestHandle(void);
    std::string getBody(const std::string &boundaryCode);
    std::string getGeneralHeaders(const std::string &boundaryCode);
    std::string getBoundaryCode(void);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const HttpRequest &httprequest);
};

#endif