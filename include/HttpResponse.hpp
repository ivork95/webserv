#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <filesystem>

#include "StatusCodes.hpp"
#include "HttpRequest.hpp"
#include "Cgi.hpp"

extern std::map<int, std::string> httpErrorCodes;

class HttpResponse
{
public:
    const HttpRequest &m_request;
    std::string m_statusLine{"HTTP/1.1 200 OK"};
    std::string m_statusCode{};
    std::string m_statusString{};
    std::string m_version{"HTTP/1.1"};
    std::map<std::string, std::string> m_headers{};
    std::string m_body{};

    // default constructor
    HttpResponse(void) = delete;

    // request constrcuctor
    HttpResponse(const HttpRequest &request);

    // copy constructor

    // copy assignment operator overload

    // destructor
    ~HttpResponse(void);

    // getters/setters
    void setBody(const std::string &path);

    // methods
    std::string responseBuild(void);
    int targetRead(const std::string &requestTarget);
    std::string targetPathCreate(const std::string &target);
    std::string resourceToStr(const std::string &path);
    void getHandle(void);
    void postHandle(void);
    void deleteHandle(void);
    void responseHandle(void);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const HttpResponse &HttpResponse);
};

#endif
