#include "HttpRequest.hpp"

// constructor
HttpRequest::HttpRequest(void)
{
    spdlog::debug("HttpRequest constructor called");
}

// destructor
HttpRequest::~HttpRequest(void)
{
    spdlog::debug("HttpRequest destructor called");
}

// getters/setters
void HttpRequest::setMethodPathVersion(void)
{
    size_t requestLineEndPos = m_rawMessage.find("\r\n");
    std::string requestLine = m_rawMessage.substr(0, requestLineEndPos);
    m_methodPathVersion = split(requestLine);
}

void HttpRequest::setBoundaryCode(void)
{
    m_boundaryCode = parseBoundaryCode(m_requestHeaders);
}

std::string HttpRequest::parseBoundaryCode(const std::map<std::string, std::string> &requestHeaders)
{
    auto contentTypeIt = requestHeaders.find("Content-Type");
    if (contentTypeIt == requestHeaders.end())
        throw std::runtime_error("Error: Content-Type not found!\n");
    std::string contentType = contentTypeIt->second;
    spdlog::info("contentType = |{}|", contentType);

    // if (!requestHeaders.contains("Content-Type"))
    // {
    //     throw std::runtime_error("Error: Content-Type not found!\n");
    // }
    // std::string contentType = requestHeaders["Content-Type"];

    std::string_view boundary{"boundary="};
    size_t boundaryStartPos = contentType.find(boundary);
    if (boundaryStartPos == std::string::npos)
    {
        throw std::runtime_error("Error: boundary= not found!\n");
    }

    return contentType.substr(boundaryStartPos + boundary.length());
}

void HttpRequest::setGeneralHeaders(void)
{
    std::string generalFieldLines = parseGeneralHeaders(m_boundaryCode);
    m_generalHeaders = fieldLinesToHeaders(generalFieldLines);
}

std::string HttpRequest::parseGeneralHeaders(const std::string &boundaryCode)
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

    return m_rawMessage.substr(BoundaryStartPos + boundaryStart.length(), (generalHeadersEndPos + generalHeadersEnd.length()) - (BoundaryStartPos + boundaryStart.length()));
}

void HttpRequest::setBody(void)
{
    m_body = parseBody(m_boundaryCode);
}

void HttpRequest::setFileName(void)
{
    m_fileName = parseFileName(m_boundaryCode);
}

// methods
std::vector<std::string> HttpRequest::split(const std::string &str) const
{
    std::vector<std::string> methodPathVersion;
    std::istringstream iss(str);
    std::string keyword;

    while (getline(iss, keyword, ' '))
        methodPathVersion.push_back(keyword);

    return methodPathVersion;
}

std::string HttpRequest::parseBody(const std::string &boundaryCode)
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

std::string HttpRequest::parseFileName(const std::string &boundaryCode)
{
    auto contentDispositionIt = m_generalHeaders.find("Content-Disposition");
    if (contentDispositionIt == m_generalHeaders.end())
    {
        throw std::runtime_error("Error: Content-Disposition not found!\n");
    }
    std::string fileNameStart{"filename="};
    size_t fileNameStartPos = contentDispositionIt->second.find(fileNameStart);
    if (fileNameStartPos == std::string::npos)
    {
        throw std::runtime_error("Error: fileNameStartPos not found!\n");
    }

    return contentDispositionIt->second.substr(fileNameStartPos + fileNameStart.length());
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const HttpRequest &httprequest)
{
    int i{};

    out << "HttpRequest (\n";
    out << "m_method = |" << httprequest.m_methodPathVersion[0] << "|\n";
    out << "m_path = |" << httprequest.m_methodPathVersion[1] << "|\n";
    out << "m_version = |" << httprequest.m_methodPathVersion[2] << "|\n";
    out << "m_body = |" << httprequest.m_body << "|\n";
    out << "m_boundaryCode = |" << httprequest.m_boundaryCode << "|\n";
    out << "m_fileName = |" << httprequest.m_fileName << "|\n";
    out << "m_generalHeaders = {\n";
    for (const auto &elem : httprequest.m_generalHeaders)
    {
        out << "[" << i << "] = (" << elem.first << ", " << elem.second << ")\n";
        i++;
    }
    out << "}\n";
    out << ")";

    return out;
}
