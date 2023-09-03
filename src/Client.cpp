#include "Client.hpp"

// serverSocket constructor
Client::Client(const TcpServer &server) : m_server(server)
{
    m_socketFd = accept(m_server.m_socketFd, (struct sockaddr *)&m_remoteaddr, &m_addrlen);
    if (m_socketFd == -1)
    {
        std::perror("accept() failed");
        throw std::runtime_error("Error: accept() failed\n");
    }

    setNonBlocking();

    // different fields in IPv4 and IPv6:
    if (m_remoteaddr.ss_family == AF_INET)
    { // IPv4
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)&m_remoteaddr;
        m_addr = &(ipv4->sin_addr);
        m_ipver = "IPv4";
        m_port = ntohs(ipv4->sin_port);
    }
    else
    { // IPv6
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)&m_remoteaddr;
        m_addr = &(ipv6->sin6_addr);
        m_ipver = "IPv6";
        m_port = ntohs(ipv6->sin6_port);
    }
    // convert the IP to a string and print it:
    inet_ntop(m_remoteaddr.ss_family, m_addr, m_ipstr, sizeof m_ipstr);

    spdlog::info("{0} constructor called", *this);
}

// destructor
Client::~Client(void)
{
    spdlog::info("{0} destructor called", *this);

    close(m_socketFd);
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const Client &client)
{
    out << "Client(" << client.m_socketFd << ": " << client.m_ipver << ": " << client.m_ipstr << ": " << client.m_port << ")";

    return out;
}

std::string Client::startLineParse(const std::string &rawMessage)
{
    // StartAndFields -> startLine
    size_t startLineEndPos = m_rawMessage.find("\r\n");
    std::string startLine = m_rawMessage.substr(0, startLineEndPos);

    spdlog::info("startLine = \n|{}|", startLine);
    return startLine;
}

void Client::setMethodPathVersion(void)
{
    spdlog::info("client->setMethodPathVersion()");

    std::istringstream stream{startLineParse(m_rawMessage)};
    std::string str{};
    std::getline(stream, str);
    std::istringstream streamTwee{str};
    streamTwee >> m_method >> m_path >> m_version;
    spdlog::info("m_method = {}", m_method);
}

std::pair<std::string, std::string> parseFieldLine(const std::string &fieldLine, const std::string &keyDelim, size_t keyDelimPos)
{
    std::string key = fieldLine.substr(0, keyDelimPos);
    std::string value = fieldLine.substr(keyDelimPos + keyDelim.length());

    return std::pair<std::string, std::string>(key, value);
}

std::map<std::string, std::string> fieldLinesToHeaders(std::string &fieldLines)
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

void Client::setHeaders(void)
{
    spdlog::info("client->setHeaders()");

    size_t startLineEndPos = m_rawMessage.find("\r\n");
    std::string fieldLines = m_rawMessage.substr(startLineEndPos + 2);

    size_t fieldsLinesEndPos = fieldLines.find("\r\n\r\n");
    fieldLines = fieldLines.substr(0, fieldsLinesEndPos + 4);
    spdlog::info("fieldLines = \n|{}|", fieldLines);

    m_headers = fieldLinesToHeaders(fieldLines);
    headersPrint(m_headers);
}

void Client::setContentLength(void)
{
    m_contentLength = std::stoi(m_headers["Content-Length"]);
    m_isStoiCalled = true;
    if (m_contentLength < 0)
        throw std::out_of_range("Negative value");
}