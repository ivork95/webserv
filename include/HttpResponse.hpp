#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>
#include <fstream>

class HttpResponse
{
public:
    std::string m_statusLine{"HTTP/1.1 200 OK"};
    std::map<std::string, std::string> m_headers{};
    /*
    std::map<std::string, std::string> m_headers = {
        {"Date", "Mon, 27 Jul 2009 12:28:53 GMT"},
        {"Server", "Apache"},
        {"Last-Modified", "Wed, 22 Jul 2009 19:15:56 GMT"},
        {"ETag", "\"34aa387-d-1568eb00\""},
        {"Accept-Ranges", "bytes"},
        {"Content-Length", "51"},
        {"Vary", "Accept-Encoding"},
        {"Content-Type", "text/plain"}};
    std::string m_body{"Hello World! My content includes a trailing CRLF."};
    */
    std::string m_body{};

    std::string responseBuild(void);
    int targetRead(const std::string &requestTarget);
    std::string targetPathCreate(const std::string &target);

    // outstream operator overload
    friend std::ostream &
    operator<<(std::ostream &out, const HttpResponse &HttpResponse);
};

#endif
