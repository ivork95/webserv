#include "HttpRequest.hpp"
#include "StatusCodes.hpp"

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
    // if (contentTypeIt == requestHeaders.end())
    //     throw std::runtime_error("HTTP/1.1 400 Bad Request");
    if (contentTypeIt == requestHeaders.end())
        throw HttpStatusCodeException(400);

    std::string contentType = contentTypeIt->second;

    std::string_view boundary{"boundary="};
    size_t boundaryStartPos = contentType.find(boundary);
    if (boundaryStartPos == std::string::npos)
        throw HttpStatusCodeException(400);
    // if (boundaryStartPos == std::string::npos)
    //     throw std::runtime_error("HTTP/1.1 400 Bad Request");

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
        throw HttpStatusCodeException(400);
    }
    // if (BoundaryStartPos == std::string::npos)
    // {
    //     throw std::runtime_error("HTTP/1.1 400 Bad Request");
    // }

    size_t generalHeadersEndPos = m_rawMessage.find(generalHeadersEnd, BoundaryStartPos + boundaryStart.length());
    if (generalHeadersEndPos == std::string::npos)
            throw HttpStatusCodeException(400);
    // if (generalHeadersEndPos == std::string::npos)
    // {
    //     throw std::runtime_error("HTTP/1.1 400 Bad Request");
    // }

    return m_rawMessage.substr(BoundaryStartPos + boundaryStart.length(), (generalHeadersEndPos + generalHeadersEnd.length()) - (BoundaryStartPos + boundaryStart.length()));
}

void HttpRequest::setBody(void)
{
    m_body = parseBody(m_boundaryCode);
}

void HttpRequest::setFileName(void)
{
    m_fileName = parseFileName(m_generalHeaders);
    strip(m_fileName);
}

void HttpRequest::strip(std::string &str)
{
    str.erase(std::remove(str.begin(), str.end(), '\"'), str.end());
}

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
        throw HttpStatusCodeException(400);
    // if (boundaryEndPos == std::string::npos)
    //     throw std::runtime_error("HTTP/1.1 400 Bad Request");

    return m_rawMessage.substr(generalHeadersEndPos + headersEnd.length(), boundaryEndPos - (generalHeadersEndPos + headersEnd.length()));
}

std::string HttpRequest::parseFileName(const std::map<std::string, std::string> &generalHeaders)
{
    auto contentDispositionIt = generalHeaders.find("Content-Disposition");
    if (contentDispositionIt == generalHeaders.end())
        throw HttpStatusCodeException(400);
    // if (contentDispositionIt == generalHeaders.end())
    //     throw std::runtime_error("HTTP/1.1 400 Bad Request");

    std::string fileNameStart{"filename="};
    size_t fileNameStartPos = contentDispositionIt->second.find(fileNameStart);
    if (fileNameStartPos == std::string::npos)
        throw HttpStatusCodeException(400);
    // if (fileNameStartPos == std::string::npos)
    //     throw std::runtime_error("HTTP/1.1 400 Bad Request");

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

// int HttpRequest::bodyToDisk(const std::string &path)
// {
//     std::ofstream outf{path};
//     if (!outf)
//         return 1;
//     outf << m_body;
//     return 0;
// }

void HttpRequest::bodyToDisk(const std::string &path)
{
    std::ofstream outf{path};
    if (!outf)
        throw HttpStatusCodeException(400);
    // if (!outf)
    //     throw std::runtime_error("HTTP/1.1 400 Bad Request");
    outf << m_body;
}