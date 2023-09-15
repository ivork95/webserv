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

class HttpRequest : public HttpMessage
{
public:
    std::vector<std::string> m_methodPathVersion{3};
    std::string m_body{};
    int m_client_max_body_size{999999};
    std::map<std::string, std::string> m_generalHeaders{};
    std::string m_boundaryCode{};
    std::string m_fileName{};

    // constructor
    HttpRequest(void);

    // copy constructor

    // copy assignment operator overload

    // destructor
    ~HttpRequest(void);

    // getters/setters
    void setMethodPathVersion(void);
    void setGeneralHeaders(void);

    void setFileName(void);
    void setBoundaryCode(void);
    void setBody(void);

    // methods
    std::vector<std::string> split(const std::string &str) const;
    std::string parseBoundaryCode(const std::map<std::string, std::string> &requestHeaders);
    std::string parseBody(const std::string &boundaryCode);
    std::string parseGeneralHeaders(const std::string &boundaryCode);
    std::string parseFileName(const std::map<std::string, std::string> &generalHeaders);
    void strip(std::string &str);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const HttpRequest &httprequest);
};

#endif