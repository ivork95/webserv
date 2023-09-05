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
