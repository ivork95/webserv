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

    while (getline(iss, keyword, ' '))
        methodPathVersion.push_back(keyword);

    return methodPathVersion;
}

std::string HttpRequest::getBoundaryCode(void)
{
    if (!m_requestHeaders.contains("Content-Type"))
    {
        throw std::runtime_error("Error: Content-Type not found!\n");
    }
    std::string contentType = m_requestHeaders["Content-Type"];

    std::string_view boundary{"boundary="};
    size_t boundaryStartPos = contentType.find(boundary);
    if (boundaryStartPos == std::string::npos)
    {
        throw std::runtime_error("Error: boundary= not found!\n");
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
    if (boundaryEndPos == std::string::npos)
    {
        throw std::runtime_error("Error: boundaryEnd not found!\n");
    }

    return m_rawMessage.substr(generalHeadersEndPos + headersEnd.length(), boundaryEndPos - (generalHeadersEndPos + headersEnd.length()));
}

std::string HttpRequest::getGeneralHeaders(const std::string &boundaryCode)
{
    const std::string boundaryStart = "--" + boundaryCode + "\r\n";
    const std::string generalHeadersEnd = "\r\n\r\n";

    size_t BoundaryStartPos = m_rawMessage.find(boundaryStart);
    if (BoundaryStartPos == std::string::npos)
    {
        throw std::runtime_error("Error: boundaryStart not found!\n");
    }

    size_t generalHeadersEndPos = m_rawMessage.find(generalHeadersEnd, BoundaryStartPos + boundaryStart.length());
    if (generalHeadersEndPos == std::string::npos)
    {
        throw std::runtime_error("Error: generalHeadersEnd not found!\n");
    }

    return m_rawMessage.substr(BoundaryStartPos + boundaryStart.length(), generalHeadersEndPos - (BoundaryStartPos + boundaryStart.length()));
}

std::string HttpRequest::getFileName(const std::string &boundaryCode)
{
    std::string generalHeaders = getGeneralHeaders(boundaryCode);
    spdlog::warn("generalHeaders = |{}|", generalHeaders);
    std::string fileNameStart{"filename="};
    size_t fileNameStartPos = generalHeaders.find(fileNameStart);
    if (fileNameStartPos == std::string::npos)
    {
        throw std::runtime_error("Error: fileNameStartPos not found!\n");
    }

    return generalHeaders.substr(fileNameStartPos + fileNameStart.length());
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const HttpRequest &httprequest)
{
    out << "HttpRequest(" << httprequest.m_methodPathVersion[0] << ")";

    return out;
}

void HttpRequest::setBoundaryCode(void)
{
    m_boundaryCode = getBoundaryCode();
    spdlog::warn("m_boundaryCode = {}", m_boundaryCode);
}

void HttpRequest::setBody(void)
{
    m_body = getBody(m_boundaryCode);
    spdlog::warn("m_body = |{}|", m_body);
}

void HttpRequest::setFileName(void)
{
    m_fileName = getFileName(m_boundaryCode);
    spdlog::warn("m_fileName = |{}|", m_fileName);
}

void HttpRequest::requestHeadersPrint(const std::map<std::string, std::string> &headers) const
{
    int i{};

    for (const auto &elem : headers)
    {
        spdlog::debug("[{0}] = ({1}, {2})", i, elem.first, elem.second);
        i++;
    }
}
