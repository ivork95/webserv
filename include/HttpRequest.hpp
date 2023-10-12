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
#include <filesystem>

#include "HttpMessage.hpp"
#include "StatusCodes.hpp"
#include "Helper.hpp"
#include "ServerConfig.hpp"
#include "HttpResponse.hpp"

class HttpResponse;

class HttpRequest : public HttpMessage
{
public:
    ServerConfig m_serverconfig{};
    LocationConfig m_locationconfig{};

    // request
    std::vector<std::string> m_methodPathVersion{3, ""};
    std::map<std::string, std::string> m_generalHeaders{};
    std::string m_boundaryCode{};
    std::string m_fileName{};
    std::string m_body{};

    // response
    int m_statusCode{};

    HttpResponse m_response{};

    // constructor
    HttpRequest(void);

    HttpRequest(const ServerConfig &serverconfig);

    // copy constructor

    // copy assignment operator overload

    // destructor
    ~HttpRequest(void);

    // getters/setters
    void methodPathVersionSet(void);
    void boundaryCodeSet(void);
    void generalHeadersSet(void);
    void fileNameSet(void);
    void bodySet(void);

    // methods
    std::string boundaryCodeParse(const std::map<std::string, std::string> &requestHeaders);
    std::string generalHeadersParse(const std::string &boundaryCode);
    std::string fileNameParse(const std::map<std::string, std::string> &generalHeaders);
    std::string bodyParse(const std::string &boundaryCode);
    void bodyToDisk(const std::string &path);
    int tokenize(const char *buf, int nbytes);
    void parse(void);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const HttpRequest &httprequest);
};

#endif