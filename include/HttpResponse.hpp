#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include "StatusCodes.hpp"

extern std::map<int, std::string> httpErrorCodes;

class HttpResponse
{
public:
    std::string m_statusLine{"HTTP/1.1 200 OK"};
    std::string m_statusCode{};
    std::string m_statusString{};
    std::string m_version{};
    std::map<std::string, std::string> m_headers{};
    std::string m_body{};

    // constructor
    HttpResponse(int statusCode);
    HttpResponse(void);
    // copy constructor

    // copy assignment operator overload
    HttpResponse& operator=(const HttpResponse &src);
    // destructor

    // getters/setters
    void setBody(const std::string &path);

    // methods
    std::string responseBuild(void);
    int targetRead(const std::string &requestTarget);
    std::string targetPathCreate(const std::string &target);
    std::string resourceToStr(const std::string &path);

    // outstream operator overload
    friend std::ostream &
    operator<<(std::ostream &out, const HttpResponse &HttpResponse);
};


#endif
