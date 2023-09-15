#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>
#include <fstream>
#include <sstream>

class HttpResponse
{
public:
    std::string m_statusLine{"HTTP/1.1 200 OK"};
    std::map<std::string, std::string> m_headers{};
    std::string m_body{};

    // constructor

    // copy constructor

    // copy assignment operator overload

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
