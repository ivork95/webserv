#include "HttpResponse.hpp"

std::string HttpResponse::responseBuild(void)
{
    std::string httpResponse = m_statusLine + "\r\n";

    for (const auto &pair : m_headers)
    {
        httpResponse += pair.first + ": " + pair.second + "\r\n";
    }
    httpResponse += "\r\n"; // End of header section
    httpResponse += m_body;
    httpResponse += "\r\n";

    return httpResponse;
}

int HttpResponse::targetRead(const std::string &requestTarget)
{
    int n{};
    std::ifstream inf{requestTarget};
    if (!inf)
        throw std::runtime_error("404 NOT FOUND\n");
    while (inf)
    {
        std::string strInput;
        std::getline(inf, strInput);
        m_body.append(strInput);
        n = n + strInput.length();
    }

    return n;
}

/*
deze functie moet beter
/index kan opgevraagd worden
/directories kunnen ook opgevraagd worden
/ kan ook opgevraagd worden
moet minsten een / zijn
...
*/
std::string HttpResponse::targetPathCreate(const std::string &target)
{
    if (!target.compare("/"))
        return "./www/index.html";

    return "./www" + target + ".html";
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const HttpResponse &httpresponse)
{
    out << "HttpResponse(" << httpresponse.m_statusLine << ")";

    return out;
}

// readResourceIntoBody
std::string HttpResponse::resourceToStr(const std::string &path)
{
    std::ifstream inf(path);
    if (!inf)
        throw std::runtime_error("HTTP/1.1 400 Bad Request");
    std::ostringstream sstr;
    sstr << inf.rdbuf();
    return sstr.str();
}

void HttpResponse::setBody(const std::string &path)
{
    m_body = resourceToStr(path);
}