#include "Request.hpp"
#include "Multiplexer.hpp"
#include "CGIPipeIn.hpp"
#include "CGIPipeOut.hpp"

Request::Request(Client &client) : m_client(client)
{
    spdlog::debug("Request serverconfig constructor called");
}

// copy constructor

// copy assignment operator overload

// destructor
Request::~Request(void)
{
    spdlog::debug("Request destructor called");
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const Request &request)
{
    int i{};

    out << "Request {\n";

    for (auto item : request.m_methodPathVersion)
    {
        out << "m_methodPathVersion[" << i << "] = |" << item << "|\n";
        i++;
    }
    // out << "m_body = |" << request.m_body << "|\n";
    out << "m_body = |We're not printing this!|\n";
    out << "m_boundaryCode = |" << request.m_boundaryCode << "|\n";
    out << "m_fileName = |" << request.m_fileName << "|\n";
    out << "m_generalHeaders = [\n";
    for (const auto &elem : request.m_generalHeaders)
    {
        out << "[" << i << "] = (" << elem.first << ", " << elem.second << ")\n";
        i++;
    }
    out << "]\n";
    out << "}\n";

    out << "m_response = " << request.m_response;

    return out;
}

void Request::methodPathVersionSet(void)
{
    size_t requestLineEndPos = m_rawMessage.find("\r\n");
    std::string requestLine = m_rawMessage.substr(0, requestLineEndPos);
    m_methodPathVersion = Helper::split(requestLine);
}

int Request::tokenize(const char *buf, int nbytes)
{
    spdlog::info("nbytes = {}", nbytes);

    m_rawMessage.append(buf, buf + nbytes);

    size_t fieldLinesEndPos = m_rawMessage.find("\r\n\r\n");
    if (fieldLinesEndPos == std::string::npos)
    {
        spdlog::warn("message incomplete [...]");
        return 1;
    }

    if (m_requestHeaders.empty())
        requestHeadersSet();

    if (m_requestHeaders.contains("Content-Length"))
    {
        if (!m_isContentLengthConverted)
            contentLengthSet();
        if (m_contentLength > static_cast<int>((m_rawMessage.length() - (fieldLinesEndPos + 4))))
        {
            spdlog::warn("Content-Length not reached [...]");
            return 2;
        }
        spdlog::info("Content-Length reached!");
    }
    // Chunked requests
    else if (m_requestHeaders.contains("Transfer-Encoding"))
    {
        if (!m_isChunked)
            m_isChunked = true;
        size_t chunkEndPos = m_rawMessage.find("\r\n0\r\n\r\n");
        if (chunkEndPos == std::string::npos)
        {
            spdlog::warn("Chunk EOF not reached [...]");
            return 3;
        }
        spdlog::info("Chunk EOF reached!");
    }

    spdlog::info("message complete!");
    return 0;
}

void Request::locationconfigSet(void)
{
    bool isLocationFound{false};
    for (const auto &location : m_client.m_server.m_serverconfig.getLocationsConfig())
    {
        if (m_methodPathVersion[1] == location.getRequestURI())
        {
            m_locationconfig = location;
            isLocationFound = true;
            break;
        }
    }
    if (!isLocationFound) // there's no matching URI
        throw StatusCodeException(404, "Error: no matching location/path found");
}

void Request::isMethodAllowed(void)
{
    auto it = find(m_locationconfig.getHttpMethods().begin(), m_locationconfig.getHttpMethods().end(), m_methodPathVersion[0]);
    if (it == m_locationconfig.getHttpMethods().end())
        throw StatusCodeException(405, "Warning: method not allowed");
}

void Request::responsePathSet(void)
{
    for (const auto &index : m_locationconfig.getIndexFile())
    {
        spdlog::debug("rootPath + index = {}", m_locationconfig.getRootPath() + index); // ? debug
        if (std::filesystem::exists(m_locationconfig.getRootPath() + index))
        {
            m_response.m_path = m_locationconfig.getRootPath() + index;
            break;
        }
    }
}

int Request::parse(void)
{
    Multiplexer &multiplexer = Multiplexer::getInstance();

    methodPathVersionSet();
    if (m_methodPathVersion.size() != 3)
        throw StatusCodeException(400, "Error: not 3 elements in request-line");
    if (m_methodPathVersion[2] != "HTTP/1.1")
        throw StatusCodeException(505, "Warning: http version not allowed");
    if ((m_methodPathVersion[0] != "GET") && (m_methodPathVersion[0] != "POST") && (m_methodPathVersion[0] != "DELETE")) // Maybe this can be removed
        throw StatusCodeException(405, "Warning: method not allowed");
    m_methodPathVersion[1] = Helper::decodePercentEncoding(m_methodPathVersion[1]);

    spdlog::critical("m_methodPathVersion[1] : {}", m_methodPathVersion[1]); // ? debug

    if (m_methodPathVersion[0] == "DELETE")
    {
        spdlog::warn("DELETE method"); // ? debug

        return deleteHandler(); // ? new
    }

    // Nasty solution to redirect + get back upload
    if (m_methodPathVersion[0] == "GET" && isImageFormat(m_methodPathVersion[1]))
    {
        return uploadHandler();
    }

    // locationconfigSet(); // Loops over location blocks and checks for match between location block and request path
    updatedLocationConfigSet(m_methodPathVersion[1]); // ? new
    isMethodAllowed();                                // For a certain location block, check if the request method is allowed
    responsePathSet();                                // For a certain location block, loops over index files, and checks if one exists

    if (m_methodPathVersion[0] == "GET")
    {
        spdlog::warn("GET method"); // ? debug

        return getHandler(); // ? new
    }

    if (m_methodPathVersion[0] == "POST")
    {
        spdlog::warn("POST method"); // ? debug

        // ? left it here because of the multiplexer
        if (!m_methodPathVersion[1].compare(0, 8, "/cgi-bin"))
        {
            spdlog::critical("POST cgi handler");
            // Hier voegen we de WRITE kant van pipe1 toe aan Epoll
            CGIPipeIn *pipein = new CGIPipeIn(m_client);
            if (multiplexer.addToEpoll(pipein, EPOLLOUT, pipein->m_pipeFd[WRITE]))
            {
                close(pipein->m_pipeFd[READ]);
                close(pipein->m_pipeFd[WRITE]);
                pipein->m_socketFd = -1;
                delete pipein;
                throw StatusCodeException(500, "Error: addToEpoll()");
            }
            return 2;
        }

        if (m_isChunked)
        {
            return chunkHandler(); // ? new
        }

        return postHandler(); // ? new
    }
}