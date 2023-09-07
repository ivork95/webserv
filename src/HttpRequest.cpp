#include "HttpRequest.hpp"

// default constructor
HttpRequest::HttpRequest(void)
{
    spdlog::debug("HttpRequest default constructor called");
}

std::string HttpRequest::requestLineParse(const std::string &rawMessage)
{
    // StartAndFields -> requestLine
    size_t requestLineEndPos = rawMessage.find("\r\n");
    std::string requestLine = rawMessage.substr(0, requestLineEndPos);

    spdlog::info("requestLine = \n|{}|", requestLine);
    return requestLine;
}

void HttpRequest::setMethodPathVersion(void)
{
    spdlog::info("HttpRequest->setMethodPathVersion()");

    std::istringstream stream{requestLineParse(m_rawMessage)};
    std::string str{};
    std::getline(stream, str);
    std::istringstream streamTwee{str};
    streamTwee >> m_method >> m_target >> m_version;
    spdlog::info("m_method = {}", m_method);
}

std::pair<std::string, std::string> HttpRequest::parseFieldLine(const std::string &fieldLine, const std::string &keyDelim, size_t keyDelimPos) const
{
    std::string key = fieldLine.substr(0, keyDelimPos);
    std::string value = fieldLine.substr(keyDelimPos + keyDelim.length());

    return std::pair<std::string, std::string>(key, value);
}

std::map<std::string, std::string> HttpRequest::fieldLinesToHeaders(std::string &fieldLines) const
{
    std::map<std::string, std::string> headers{};
    std::string fieldLine{};
    std::string fieldLineDelim{"\r\n"};
    size_t fieldLineDelimPos{};
    std::string keyDelim = ": ";
    size_t keyDelimPos{};

    while ((fieldLineDelimPos = fieldLines.find(fieldLineDelim)) != std::string::npos)
    {
        fieldLine = fieldLines.substr(0, fieldLineDelimPos);
        if ((keyDelimPos = fieldLine.find(keyDelim)) != std::string::npos)
            headers.insert(parseFieldLine(fieldLine, keyDelim, keyDelimPos));
        fieldLines.erase(0, fieldLineDelimPos + fieldLineDelim.length());
    }

    return headers;
}

void HttpRequest::headersPrint(const std::map<std::string, std::string> &headers)
{
    int i{};

    for (const auto &elem : headers)
    {
        spdlog::info("[{0}] = ({1}, {2})", i, elem.first, elem.second);
        i++;
    }
}

void HttpRequest::setHeaders(void)
{
    spdlog::info("HttpRequest->setHeaders()");
    size_t requestLineEndPos = m_rawMessage.find("\r\n");
    std::string fieldLines = m_rawMessage.substr(requestLineEndPos + 2, m_fieldLinesEndPos + 4);
    spdlog::info("fieldLines = \n|{}|", fieldLines);

    m_headers = fieldLinesToHeaders(fieldLines);
    headersPrint(m_headers);
}

void HttpRequest::setContentLength(void)
{
    m_contentLength = std::stoi(m_headers["Content-Length"]);
    isContentLengthConverted = true;
    if (m_contentLength < 0)
        throw std::out_of_range("Negative value");
}

std::string HttpRequest::postRequestHandle(void)
{
    spdlog::info("POST method");

    if (m_headers.find("Content-Length") == m_headers.end())
        return "HTTP/1.1 411 Length Required";

    if (!isContentLengthConverted)
    {
        spdlog::info("std::stoi()");
        try
        {
            setContentLength();
        }
        catch (...)
        {
            return "HTTP/1.1 400 Bad Request";
        }
    }

    if (m_contentLength > m_client_max_body_size)
        return "HTTP/1.1 413 Payload Too Large";

    if (m_contentLength > static_cast<int>((m_rawMessage.length() - (m_fieldLinesEndPos + 4))))
    {
        spdlog::warn("Content-Length not reached [...]");
        return "";
    }

    // Create and send response
    spdlog::info("Content-Length reached!");
    return "HTTP/1.1 200 OK";
}

std::string HttpRequest::getBoundaryCode(void)
{
    if (m_headers.find("Content-Type") == m_headers.end())
    {
        throw;
    }
    std::string contentType = m_headers["Content-Type"];

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