#include "HttpRequest.hpp"

// constructor
HttpRequest::HttpRequest(std::string rawMessage, std::map<std::string, std::string> requestHeaders, int contentLength) : m_rawMessage(rawMessage), m_requestHeaders(requestHeaders), m_contentLength(contentLength)
{
    size_t requestLineEndPos = m_rawMessage.find("\r\n");
    std::string requestLine = m_rawMessage.substr(0, requestLineEndPos);
    m_methodPathVersion = split(requestLine);

    spdlog::debug("{} constructor called", *this);
}

std::vector<std::string> HttpRequest::split(const std::string &str)
{
    std::vector<std::string> methodPathVersion;
    std::istringstream iss(str);
    std::string keyword;

    while (getline(iss, keyword))
    {
        methodPathVersion.push_back(keyword);
    }

    return methodPathVersion;
}

std::string HttpRequest::postRequestHandle(void)
{
    spdlog::info("POST method");

    if (m_requestHeaders.find("Content-Length") == m_requestHeaders.end())
        return "HTTP/1.1 411 Length Required";

    if (m_contentLength > m_client_max_body_size)
        return "HTTP/1.1 413 Payload Too Large";

    // Create and send response
    spdlog::info("Content-Length reached!");
    return "HTTP/1.1 200 OK";
}

std::string HttpRequest::getBoundaryCode(void)
{
    if (m_requestHeaders.find("Content-Type") == m_requestHeaders.end())
    {
        throw;
    }
    std::string contentType = m_requestHeaders["Content-Type"];

    std::string_view boundary{"boundary="};
    size_t boundaryStartPos = contentType.find(boundary);
    if (boundaryStartPos == std::string::npos)
    {
        throw;
    }

    return contentType.substr(boundaryStartPos + boundary.length());
}

std::string HttpRequest::getBody(const std::string &boundaryCode)
{
    const std::string headersEnd = "\r\n\r\n";
    size_t requestHeadersEndPos = m_rawMessage.find(headersEnd);
    size_t generalHeadersEndPos = m_rawMessage.find(headersEnd, requestHeadersEndPos + 1);

    const std::string boundaryEnd = "\r\n--" + boundaryCode + "--\r\n";
    size_t boundaryEndPos = m_rawMessage.find(boundaryEnd);

    return m_rawMessage.substr(generalHeadersEndPos + headersEnd.length(), boundaryEndPos - (generalHeadersEndPos + headersEnd.length()));
}

std::string HttpRequest::getGeneralHeaders(const std::string &boundaryCode)
{
    const std::string boundaryStart = "--" + boundaryCode + "\r\n";
    const std::string generalHeadersEnd = "\r\n\r\n";

    size_t BoundaryStartPos = m_rawMessage.find(boundaryStart);
    if (BoundaryStartPos == std::string::npos)
    {
        std::cerr << "Boundary not found!" << std::endl;
    }

    size_t generalHeadersEndPos = m_rawMessage.find(generalHeadersEnd, BoundaryStartPos + boundaryStart.length());
    if (generalHeadersEndPos == std::string::npos)
    {
        std::cerr << "Second boundary not found!" << std::endl;
    }

    return m_rawMessage.substr(BoundaryStartPos + boundaryStart.length(), generalHeadersEndPos - (BoundaryStartPos + boundaryStart.length()));
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const HttpRequest &httprequest)
{
    out << "HttpRequest(" << httprequest.m_methodPathVersion[0] << ")";

    return out;
}