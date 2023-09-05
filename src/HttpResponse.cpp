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
    std::ifstream inf{requestTarget};
    if (!inf)
        throw std::runtime_error("404 NOT FOUND\n");
    while (inf)
    {
        std::string strInput;
        std::getline(inf, strInput);
        m_body.append(strInput);
    }

    return 0;
}

std::string HttpResponse::targetPathCreate(const std::string &target)
{
    for (auto c : target)
    {
        if (c != '/')
            return ("./www" + target);
    }

    return ("./www/index.html");
}