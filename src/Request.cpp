#include "Request.hpp"
#include "Multiplexer.hpp"
#include "CGIPipeIn.hpp"
#include "CGIPipeOut.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

// constructor
Request::Request(Client &client) : m_client(client), m_pipein(client), m_pipeout(client, *this, m_response)
{
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

void Request::updatedLocationConfigSet(const std::string &originalMethodPath)
{
    std::string methodPath;
    std::string queryString;

    bool isLocationFound{false};
    std::string::size_type pos = originalMethodPath.find('?');
    if (pos == std::string::npos)
    {
        methodPath = originalMethodPath;
    }
    else
    {
        methodPath = originalMethodPath.substr(0, pos);
        queryString = originalMethodPath.substr(pos + 1);
        std::stringstream ss(queryString);
        while (ss.good())
        {
            std::string substr;
            getline(ss, substr, '&');
            m_query.push_back(substr);
        }
    }
    for (const auto &location : m_client.getServer().m_serverconfig.getLocationsConfig())
    {
        if (methodPath == location.getRequestURI())
        {
            m_locationconfig = location;
            isLocationFound = true;
            break;
        }
    }
    if (!isLocationFound) // there's no matching URI
        throw StatusCodeException(404, "Warning: no matching location/path found");
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
        spdlog::info("m_locationconfig.getRootPath() + index = {}", m_locationconfig.getRootPath() + index);
        if (std::filesystem::exists(m_locationconfig.getRootPath() + index))
        {
            m_response.pathSet(m_locationconfig.getRootPath() + index);
            break;
        }
    }
}

int Request::tokenize(const char *buf, int nbytes)
{
    m_rawMessage.append(buf, buf + nbytes);

    size_t fieldLinesEndPos = m_rawMessage.find("\r\n\r\n");
    if (fieldLinesEndPos == std::string::npos)
    {
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
            return 2;
        }
    }
    else if (m_requestHeaders.contains("Transfer-Encoding")) // Chunked request
    {
        if (!m_isChunked)
            m_isChunked = true;
        size_t chunkEndPos = m_rawMessage.find("\r\n0\r\n\r\n");
        if (chunkEndPos == std::string::npos)
        {
            return 3;
        }
    }

    spdlog::debug("HTTP Message complete:\n{}", m_rawMessage);

    return 0;
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

    if (m_methodPathVersion[0] == "DELETE")
        return deleteHandler();

    // Nasty solution to redirect + get back upload
    if (m_methodPathVersion[0] == "GET" && Helper::isImageFormat(m_methodPathVersion[1]))
        return uploadHandler();

    updatedLocationConfigSet(m_methodPathVersion[1]);
    isMethodAllowed(); // For a certain location block, check if the request method is allowed
    responsePathSet(); // For a certain location block, loops over index files, and checks if one exists

    if (m_methodPathVersion[0] == "GET")
    {
        if (!m_methodPathVersion[1].compare(0, 8, "/cgi-bin"))
        {
            std::cerr << "ADDING PIPEOUT FD TO EPOLLIN: " << m_pipeout.m_pipeFd[READ] << std::endl;
            if (multiplexer.addToEpoll(&m_pipeout, EPOLLIN, m_pipeout.m_pipeFd[READ]))
                throw StatusCodeException(500, "Error: EPOLL_CTL_MOD failed");
            m_pipeout.forkCloseDupExec();
            return 2;
        }
        return getHandler();
    }
    else
    {
        if (!m_methodPathVersion[1].compare(0, 8, "/cgi-bin"))
        {
            m_client.getRequest().bodySet();
            std::cerr << "ADDING PIPEIN FD TO EPOLLOUT: " << m_pipein.m_pipeFd[WRITE] << std::endl;
            if (multiplexer.addToEpoll(&m_pipein, EPOLLOUT, m_pipein.m_pipeFd[WRITE])) // Add the WRITE end of pipein to Epoll
                throw StatusCodeException(500, "addToEpoll()", errno);

            return 2;
        }
        if (m_isChunked)
            return chunkHandler();

        return postHandler();
    }
}