#include "HttpRequest.hpp"

// constructor
HttpRequest::HttpRequest(void)
{
    spdlog::debug("HttpRequest default constructor called");
}

HttpRequest::HttpRequest(const ServerConfig &serverconfig) : m_serverconfig(serverconfig)
{
    spdlog::debug("HttpRequest serverconfig constructor called");
}

// copy constructor

// copy assignment operator overload

// destructor
HttpRequest::~HttpRequest(void)
{
    spdlog::debug("HttpRequest destructor called");
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const HttpRequest &httprequest)
{
    int i{};

    out << "HttpRequest (\n";

    for (auto item : httprequest.m_methodPathVersion)
    {
        out << "m_methodPathVersion[" << i << "] = |" << item << "|\n";
        i++;
    }
    // out << "m_body = |" << httprequest.m_body << "|\n";
    out << "m_body = |We're not printing this!|\n";
    out << "m_boundaryCode = |" << httprequest.m_boundaryCode << "|\n";
    out << "m_fileName = |" << httprequest.m_fileName << "|\n";
    out << "m_statusCode = |" << httprequest.m_statusCode << "|\n";
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

void HttpRequest::methodPathVersionSet(void)
{
    size_t requestLineEndPos = m_rawMessage.find("\r\n");
    std::string requestLine = m_rawMessage.substr(0, requestLineEndPos);
    m_methodPathVersion = Helper::split(requestLine);
}

std::string HttpRequest::boundaryCodeParse(const std::map<std::string, std::string> &requestHeaders)
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

void HttpRequest::boundaryCodeSet(void)
{
    m_boundaryCode = boundaryCodeParse(m_requestHeaders);
}

std::string HttpRequest::generalHeadersParse(const std::string &boundaryCode)
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

void HttpRequest::generalHeadersSet(void)
{
    std::string generalFieldLines = generalHeadersParse(m_boundaryCode);
    m_generalHeaders = fieldLinesToHeaders(generalFieldLines);
}

std::string HttpRequest::bodyParse(const std::string &boundaryCode)
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

void HttpRequest::bodySet(void)
{
    m_body = bodyParse(m_boundaryCode);
}

std::string HttpRequest::fileNameParse(const std::map<std::string, std::string> &generalHeaders)
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

void HttpRequest::fileNameSet(void)
{
    m_fileName = fileNameParse(m_generalHeaders);
    Helper::strip(m_fileName);
}

int HttpRequest::tokenize(const char *buf, int nbytes)
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
		spdlog::critical("chunkEndPos = {}", chunkEndPos);
		if (chunkEndPos == std::string::npos) {
			spdlog::warn("Chunk EOF not reached [...]");
            return 3;
		}
        spdlog::info("Chunk EOF reached!");
    }

    spdlog::info("message complete!");
    spdlog::info("[after] m_rawMessage = \n|{}|", m_rawMessage);

    return 0;
}

void HttpRequest::parse(void)
{
    methodPathVersionSet();
	spdlog::critical("tokenized request = {}", *this);
    if (m_methodPathVersion.size() != 3)
        throw StatusCodeException(400, "Error: not 3 elements in request-line");
    if (m_methodPathVersion[2] != "HTTP/1.1")
        throw StatusCodeException(505, "Warning: http version not allowed");
    if ((m_methodPathVersion[0] != "GET") && (m_methodPathVersion[0] != "POST") && (m_methodPathVersion[0] != "DELETE")) // Maybe this can be removed
        throw StatusCodeException(405, "Warning: method not allowed");

    // Percentage decode URI string
    m_methodPathVersion[1] = Helper::decodePercentEncoding(m_methodPathVersion[1]);

    bool isLocationFound{false};
    for (auto &location : m_serverconfig.getLocationsConfig()) // loop over location blocks
    {
        if (m_methodPathVersion[1] == location.getRequestURI()) // als location URI matched met request path
        {
            spdlog::critical("_requestURI = {}", location.getRequestURI());

            m_locationconfig = location;
            isLocationFound = true;
            break;
        }
    }
    if (!isLocationFound) // there's no matching URI
    {
        throw StatusCodeException(404, "Error: ifstream1");
    }
	spdlog::critical("_methodPathVersion[1] = {}", m_methodPathVersion[1]);

    bool isMethodFound{false};
    for (auto &httpmethods : m_locationconfig.getHttpMethods())
    {
        if (m_methodPathVersion[0] == httpmethods)
        {
            isMethodFound = true;
            break;
        }
    }
    if (!isMethodFound)
        throw StatusCodeException(405, "Warning: method not allowed");

    if (m_methodPathVersion[0] == "POST")
    {
		// Parse chunked request
		if (m_isChunked) {

			// spdlog::critical("TE found!"); // ? debug

			// Check for "chunked" directive
			for (auto header: m_requestHeaders) {
				// spdlog::critical("header[0] = {}", header.first); // ? debug
				// spdlog::critical("header[1] = {}", header.second); // ? debug
				if (header.first == "Transfer-Encoding") {
					if (header.second != "chunked") {
						throw StatusCodeException(400, "Warning: Invalid TE form");
					}

					// spdlog::critical("Chunky boi found!"); // ? debug
				}
			}
			// spdlog::critical("rawMessage = {}", m_rawMessage); // ? debug

			// Extract chunk body
			const std::string headersEnd = "\r\n\r\n";
			const std::string chunkEnd = "\r\n0\r\n\r\n";
			size_t requestHeadersEndPos = m_rawMessage.find(headersEnd); // ? debug
			size_t generalHeadersEndPos = m_rawMessage.find(chunkEnd); // ? debug

			// spdlog::critical("requestHeadersEndPos = {}", requestHeadersEndPos); // ? debug
			// spdlog::critical("generalHeadersEndPos = {}", generalHeadersEndPos); // ? debug

			std::string chunkedBody = m_rawMessage.substr(requestHeadersEndPos + 4, generalHeadersEndPos - requestHeadersEndPos);
			
			// spdlog::critical("chunkedBody = {}", chunkedBody); // ? debug

			// Split lines
			std::vector<std::string>	chunkLines{};
			std::istringstream			iss(chunkedBody);
			std::string					token{};
			while (std::getline(iss, token))
				chunkLines.push_back(token);
			
			// for (auto line: chunkLines) {
			// 	spdlog::critical("line = {}", line); // ? debug
			// }

			spdlog::critical("nb chunks = {}", chunkLines.size()); // ? debug

			// Loop over chunk lines
			size_t i = 0;
			int chunkSize{};
			int	lineSize{};
			while (i < chunkLines.size()) {
				spdlog::critical("current line = {}", chunkLines[i]); // ? debug
				std::string currentLine = chunkLines[i];

				// Convert hex chunk size to int
				chunkSize = Helper::hexToInt(currentLine);
				spdlog::warn("hex chunkSize to int = {}", chunkSize); // ? debug

				// Reached the end of chunk
				if (chunkSize == 0) {
					if (i != chunkLines.size() - 1) {
						throw StatusCodeException(400, "Warning: chunk body not entirely read");
					}
					spdlog::critical("i = {}", i);
					break ;
				}

				// Get the length of the following line
				lineSize = chunkLines[i + 1].size() - 1;

				spdlog::critical("comp chunkSize = {}" , chunkSize); // ? debug
				spdlog::critical("comp lineSize = {}" , lineSize); // ? debug

				// Compare chunk and line sizes
				if (chunkSize != lineSize)
					throw StatusCodeException(400, "Warning: chunk size is different from the chunk line length");
				i += 2;
			}

			// Set body
			m_body = chunkedBody;

			return ;

		} else {
			if (m_contentLength > m_locationconfig.getClientMaxBodySize())
				throw StatusCodeException(413, "Warning: contentLength larger than max_body_size");

			boundaryCodeSet();
			generalHeadersSet();
			fileNameSet();
			bodySet();
		}
    }
	spdlog::critical("parsed request = {}", *this);
}