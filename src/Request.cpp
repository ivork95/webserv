#include "Request.hpp"
#include "Multiplexer.hpp"
#include "CGIPipeIn.hpp"

// constructor
Request::Request(void)
{
    spdlog::debug("Request default constructor called");
}

Request::Request(const ServerConfig &serverconfig) : m_serverconfig(serverconfig)

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
    out << "m_statusCode = |" << request.m_statusCode << "|\n";
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

std::string Request::boundaryCodeParse(const std::map<std::string, std::string> &requestHeaders)
{
    auto contentTypeIt = requestHeaders.find("Content-Type");
    if (contentTypeIt == requestHeaders.end())
        throw StatusCodeException(400, "Error: missing Content-Type header");

    std::string contentType = contentTypeIt->second;

    std::string_view boundary{"boundary="};
    size_t boundaryStartPos = contentType.find(boundary);
    if (boundaryStartPos == std::string::npos)
        throw StatusCodeException(400, "Error: missing boundary=");

    return contentType.substr(boundaryStartPos + boundary.length());
}

void Request::boundaryCodeSet(void)
{
    m_boundaryCode = boundaryCodeParse(m_requestHeaders);
}

std::string Request::generalHeadersParse(const std::string &boundaryCode)
{
    const std::string boundaryStart = "--" + boundaryCode + "\r\n";
    const std::string generalHeadersEnd = "\r\n\r\n";

    size_t BoundaryStartPos = m_rawMessage.find(boundaryStart);
    if (BoundaryStartPos == std::string::npos)
    {
        throw StatusCodeException(400, "Error: invalid Content-Length header2");
    }

    size_t generalHeadersEndPos = m_rawMessage.find(generalHeadersEnd, BoundaryStartPos + boundaryStart.length());
    if (generalHeadersEndPos == std::string::npos)
        throw StatusCodeException(400, "Error: invalid Content-Length header3");

    return m_rawMessage.substr(BoundaryStartPos + boundaryStart.length(), (generalHeadersEndPos + generalHeadersEnd.length()) - (BoundaryStartPos + boundaryStart.length()));
}

void Request::generalHeadersSet(void)
{
    std::string generalFieldLines = generalHeadersParse(m_boundaryCode);
    m_generalHeaders = fieldLinesToHeaders(generalFieldLines);
}

std::string Request::bodyParse(const std::string &boundaryCode)
{
    const std::string headersEnd = "\r\n\r\n";
    size_t requestHeadersEndPos = m_rawMessage.find(headersEnd);
    size_t generalHeadersEndPos = m_rawMessage.find(headersEnd, requestHeadersEndPos + 1);

    if (boundaryCode.empty())
    {
        requestHeadersEndPos += 4;
        spdlog::debug("M_BODY NO BOUNDRY {}", m_rawMessage.substr(requestHeadersEndPos + 4, m_contentLength));
        return (m_rawMessage.substr(requestHeadersEndPos + 4, m_contentLength));
    }

    const std::string boundaryEnd = "\r\n--" + boundaryCode + "--\r\n";
    size_t boundaryEndPos = m_rawMessage.find(boundaryEnd);
    if (boundaryEndPos == std::string::npos)
        throw StatusCodeException(400, "Error: couldn't find boundaryEnd");

    return m_rawMessage.substr(generalHeadersEndPos + headersEnd.length(), boundaryEndPos - (generalHeadersEndPos + headersEnd.length()));
}

void Request::bodySet(void)
{
    m_body = bodyParse(m_boundaryCode);
}

void Request::bodyToDisk(const std::string &path)
{
    std::ofstream outf{path};
    if (!outf)
        throw StatusCodeException(400, "Error: ofstream");
    outf << m_body;
}

std::string Request::fileNameParse(const std::map<std::string, std::string> &generalHeaders)
{
    auto contentDispositionIt = generalHeaders.find("Content-Disposition");
    if (contentDispositionIt == generalHeaders.end())
        throw StatusCodeException(400, "Error: couldn't find Content-Disposition");

    std::string fileNameStart{"filename="};
    size_t fileNameStartPos = contentDispositionIt->second.find(fileNameStart);
    if (fileNameStartPos == std::string::npos)
        throw StatusCodeException(400, "Error: couldn't find filename=");

    return contentDispositionIt->second.substr(fileNameStartPos + fileNameStart.length());
}

void Request::fileNameSet(void)
{
    m_fileName = fileNameParse(m_generalHeaders);
    Helper::strip(m_fileName);
}

int Request::tokenize(const char *buf, int nbytes)
{
    // We got some good data from a client
    spdlog::info("nbytes = {}", nbytes);

    m_rawMessage.append(buf, buf + nbytes);

    // spdlog::info("[before] m_rawMessage = \n|{}|", m_rawMessage);

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

void Request::parse(void)
{
    methodPathVersionSet();
    if (m_methodPathVersion.size() != 3)
        throw StatusCodeException(400, "Error: not 3 elements in request-line");
    if (m_methodPathVersion[2] != "HTTP/1.1")
        throw StatusCodeException(505, "Warning: http version not allowed");
    if ((m_methodPathVersion[0] != "GET") && (m_methodPathVersion[0] != "POST") && (m_methodPathVersion[0] != "DELETE")) // Maybe this can be removed
        throw StatusCodeException(405, "Warning: method not allowed");

    // Percentage decode URI string
    m_methodPathVersion[1] = Helper::decodePercentEncoding(m_methodPathVersion[1]);

    // Nasty solution to redirect + get back upload
    if (m_methodPathVersion[1].ends_with("jpg") || m_methodPathVersion[1].ends_with("jpeg") || m_methodPathVersion[1].ends_with("png"))
    {
        m_response.bodySet("./www" + m_methodPathVersion[1]);
        m_response.m_statusCode = 200;
        return;
    }

    // Loops over location blocks and checks for match between location block and request path
    bool isLocationFound{false};
    for (const auto &location : m_serverconfig.getLocationsConfig())
    {
        if (m_methodPathVersion[1] == location.getRequestURI())
        {
            m_locationconfig = location;
            isLocationFound = true;
            break;
        }
    }
    if (!isLocationFound) // there's no matching URI
    {
        throw StatusCodeException(404, "Error: no matching location/path found");
    }

    // For a certain location block, check if the request method is allowed
    auto it = find(m_locationconfig.getHttpMethods().begin(), m_locationconfig.getHttpMethods().end(), m_methodPathVersion[0]);
    if (it == m_locationconfig.getHttpMethods().end())
        throw StatusCodeException(405, "Warning: method not allowed");

    // For a certain location block, loops over index files, and checks if one exists
    bool isIndexFileFound{false};
    for (const auto &index : m_locationconfig.getIndexFile())
    {
        spdlog::debug("rootPath + index = {}", m_locationconfig.getRootPath() + index);
        if (std::filesystem::exists(m_locationconfig.getRootPath() + index))
        {
            m_response.m_path = m_locationconfig.getRootPath() + index;
            isIndexFileFound = true;
            break;
        }
    }
    if (!isIndexFileFound)
        throw StatusCodeException(404, "Error: no matching index file found");

    if (m_methodPathVersion[0] == "POST")
    {
        // Hier voegen we de WRITE kant van pipe1 toe aan Epoll
        Multiplexer &multiplexer = Multiplexer::getInstance();
        CGIPipeIn *pipein = new CGIPipeIn;
        if (pipe(pipein->m_pipeFd))
            throw StatusCodeException(500, "Error: pipe()");
        struct epoll_event ev
        {
        };
        ev.data.ptr = pipein;
        ev.events = EPOLLOUT;
        if (epoll_ctl(multiplexer.m_epollfd, EPOLL_CTL_ADD, pipein->m_pipeFd[1], &ev) == -1)
            throw StatusCodeException(500, "Error: EPOLL_CTL_MOD failed");
        return;

        // Parse chunked request
        if (m_isChunked)
        {
            // Check for "chunked" directive
            chunkHeadersParse();

            // Extract chunk body
            chunkBodyExtract();

            // Tokenize body to separate len and actual chunk
            chunkBodyTokenize();

            // Set body
            chunkBodySet();
            spdlog::warn("m_chunkBody = {}", m_chunkBody);

            // ! To change I guess
            m_body = m_chunkBody;
        }
        else
        {
            if (m_contentLength > m_locationconfig.getClientMaxBodySize())
                throw StatusCodeException(413, "Warning: contentLength larger than max_body_size");

            boundaryCodeSet();
            generalHeadersSet();
            fileNameSet();
            bodySet();

            // Needs to be moved - Upload Post handeling
            if (m_fileName.empty())
                throw StatusCodeException(400, "Error: no fileName");
            bodyToDisk("./www/" + m_fileName);
            m_response.m_headers.insert({"Location", "/" + Helper::percentEncode(m_fileName)});
            m_response.m_statusCode = 303;
            return;
        }
    }
    m_response.bodySet(m_response.m_path);
    m_response.m_statusCode = 200;
}