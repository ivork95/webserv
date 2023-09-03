#include "HttpRequest.hpp"

// default constructor
HttpRequest::HttpRequest(void)
{
    spdlog::debug("HttpRequest default constructor called");
}

std::string HttpRequest::startLineParse(const std::string &rawMessage)
{
    // StartAndFields -> startLine
    size_t startLineEndPos = rawMessage.find("\r\n");
    std::string startLine = rawMessage.substr(0, startLineEndPos);

    spdlog::info("startLine = \n|{}|", startLine);
    return startLine;
}

void HttpRequest::setMethodPathVersion(void)
{
    spdlog::info("HttpRequest->setMethodPathVersion()");

    std::istringstream stream{startLineParse(m_rawMessage)};
    std::string str{};
    std::getline(stream, str);
    std::istringstream streamTwee{str};
    streamTwee >> m_method >> m_path >> m_version;
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

void headersPrint(const std::map<std::string, std::string> &headers)
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

    size_t startLineEndPos = m_rawMessage.find("\r\n");
    std::string fieldLines = m_rawMessage.substr(startLineEndPos + 2);

    fieldLines = fieldLines.substr(0, m_fieldLinesEndPos + 4);
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

int HttpRequest::postRequestHandle(void)
{
    spdlog::info("POST method");
    int response{};

    if (m_headers.find("Content-Length") == m_headers.end())
        response = 411;

    if (!isContentLengthConverted)
    {
        spdlog::info("std::stoi()");
        try
        {
            setContentLength();
        }
        catch (...)
        {
            response = 400;
        }
    }

    if (m_contentLength > m_client_max_body_size)
        response = 413;

    if (m_contentLength > static_cast<int>((m_rawMessage.length() - (m_fieldLinesEndPos + 4))))
    {
        spdlog::warn("Content-Length not reached [...]");
        return 0;
    }
    // Create and send response
    spdlog::info("Content-Length reached!");
    response = 200;

    return response;
}