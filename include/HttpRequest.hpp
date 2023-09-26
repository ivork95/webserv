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
    void methodPathVersionSet(void);
    void generalHeadersSet(void);
    void fileNameSet(void);
    void boundaryCodeSet(void);
    void bodySet(void);

    // methods
    std::string boundaryCodeParse(const std::map<std::string, std::string> &requestHeaders);
    std::string bodyParse(const std::string &boundaryCode);
    std::string generalHeadersParse(const std::string &boundaryCode);
    std::string fileNameParse(const std::map<std::string, std::string> &generalHeaders);
    std::vector<std::string> split(const std::string &str) const;
    void strip(std::string &str) const;
    int tokenize(const char *buf, int nbytes);
    void parse(void);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const HttpRequest &httprequest);
};

#endif