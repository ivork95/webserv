#include "Message.hpp"

// constructor
Message::Message(void)
{
    // spdlog::debug("Message constructor called");
    Logger::getInstance().debug("Message constructor called");
}

// copy constructor

// copy assignment operator

// destructor
Message::~Message(void)
{
    // spdlog::debug("Message destructor called");
    Logger::getInstance().debug("Message destructor called");
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const Message &message)
{
    int i{};

    out << "Message (\n";
    out << "m_rawMessage = |" << message.m_rawMessage << "|\n";
    out << "m_requestHeaders = {\n";
    for (const auto &elem : message.m_requestHeaders)
    {
        out << "[" << i << "] = (" << elem.first << ", " << elem.second << ")\n";
        i++;
    }
    out << "}\n";
    out << "m_contentLength = " << message.m_contentLength << '\n';
    out << "m_isContentLengthConverted = " << message.m_isContentLengthConverted << '\n';
    out << ")";

    return out;
}

// getters/setters
void Message::requestHeadersSet(void)
{
    size_t requestLineEndPos = m_rawMessage.find("\r\n");
    size_t fieldLinesEndPos = m_rawMessage.find("\r\n\r\n");
    std::string fieldLines = m_rawMessage.substr(requestLineEndPos + 2, (fieldLinesEndPos + 4) - (requestLineEndPos + 2));

    m_requestHeaders = fieldLinesToHeaders(fieldLines);
}

void Message::contentLengthSet(void)
{
    try
    {
        m_contentLength = std::stoi(m_requestHeaders["Content-Length"]);
        m_isContentLengthConverted = true;
    }
    catch (...)
    {
        throw StatusCodeException(400, "Error: invalid Content-Length header1");
    }
}

// methods
std::pair<std::string, std::string> Message::fieldLineParse(const std::string &fieldLine, const std::string &keyDelim, size_t keyDelimPos) const
{
    std::string key = fieldLine.substr(0, keyDelimPos);
    std::string value = fieldLine.substr(keyDelimPos + keyDelim.length());

    return std::pair<std::string, std::string>(key, value);
}

std::map<std::string, std::string> Message::fieldLinesToHeaders(std::string &fieldLines)
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
            headers.insert(fieldLineParse(fieldLine, keyDelim, keyDelimPos));
        fieldLines.erase(0, fieldLineDelimPos + fieldLineDelim.length());
    }

    return headers;
}

void Message::requestHeadersPrint(const std::map<std::string, std::string> &headers) const
{
    int i{};

    for (const auto &elem : headers)
    {
        // spdlog::debug("[{0}] = ({1}, {2})", i, elem.first, elem.second);
        Logger::getInstance().debug("[" + std::to_string(i) + "] = (" + elem.first + ", " + elem.second + ")");
        i++;
    }
}