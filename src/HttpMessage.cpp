#include "HttpMessage.hpp"

// constructor
HttpMessage::HttpMessage(void)
{
    spdlog::debug("HttpMessage constructor called");
}

// copy constructor

// destructor
HttpMessage::~HttpMessage(void)
{
    spdlog::debug("HttpMessage destructor called");
}

// getters/setters
void HttpMessage::setRequestHeaders(void)
{
    size_t requestLineEndPos = m_rawMessage.find("\r\n");
    size_t fieldLinesEndPos = m_rawMessage.find("\r\n\r\n");
    std::string fieldLines = m_rawMessage.substr(requestLineEndPos + 2, (fieldLinesEndPos + 4) - (requestLineEndPos + 2));

    m_requestHeaders = fieldLinesToHeaders(fieldLines);
}

void HttpMessage::setContentLength(void)
{
    try
    {
        m_contentLength = std::stoi(m_requestHeaders["Content-Length"]);
        m_isContentLengthConverted = true;
    }
    catch (...)
    {
        m_contentLength = -1;
    }
}

// methods
std::pair<std::string, std::string> HttpMessage::parseFieldLine(const std::string &fieldLine, const std::string &keyDelim, size_t keyDelimPos) const
{
    std::string key = fieldLine.substr(0, keyDelimPos);
    std::string value = fieldLine.substr(keyDelimPos + keyDelim.length());

    return std::pair<std::string, std::string>(key, value);
}

std::map<std::string, std::string> HttpMessage::fieldLinesToHeaders(std::string &fieldLines)
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

void HttpMessage::requestHeadersPrint(const std::map<std::string, std::string> &headers) const
{
    int i{};

    for (const auto &elem : headers)
    {
        spdlog::debug("[{0}] = ({1}, {2})", i, elem.first, elem.second);
        i++;
    }
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const HttpMessage &httpmessage)
{
    int i{};

    out << "HttpMessage (\n";
    out << "m_rawMessage = |" << httpmessage.m_rawMessage << "|\n";
    out << "m_requestHeaders = {\n";
    for (const auto &elem : httpmessage.m_requestHeaders)
    {
        out << "[" << i << "] = (" << elem.first << ", " << elem.second << ")\n";
        i++;
    }
    out << "}\n";
    out << "m_contentLength = " << httpmessage.m_contentLength << '\n';
    out << "m_isContentLengthConverted = " << httpmessage.m_isContentLengthConverted << '\n';
    out << ")";

    return out;
}