#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <map>
#include <string>
#include <sstream>

#include "HttpMessage.hpp"
#include "StatusCodes.hpp"

class HttpRequest : public HttpMessage
{
public:
    std::vector<std::string> m_methodPathVersion{3};
    std::string m_body{};
    int m_client_max_body_size{999999};
    std::string m_boundaryCode{};
    std::string m_fileName{};
    std::map<std::string, std::string> m_generalHeaders{};
    int m_statusCode{};

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
    std::string parseBoundaryCode(const std::map<std::string, std::string> &requestHeaders);
    std::string parseBody(const std::string &boundaryCode);
    std::string parseGeneralHeaders(const std::string &boundaryCode);
    std::string parseFileName(const std::map<std::string, std::string> &generalHeaders);
    std::vector<std::string> split(const std::string &str) const;
    void strip(std::string &str);
    void bodyToDisk(const std::string &path);
    int tokenize(const char *buf, int nbytes);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const HttpRequest &httprequest);
};

#endif