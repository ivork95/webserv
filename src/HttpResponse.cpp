#include "HttpResponse.hpp"

// request constructor
HttpResponse::HttpResponse(const HttpRequest &request) : m_request(request), m_statusCode(request.m_statusCode)
{
    spdlog::debug("HttpResponse() constructor with request called");
}

// copy constructor

// copy assignment operator overload

// destructor
HttpResponse::~HttpResponse(void)
{
    spdlog::debug("HttpResponse() destructor called");
}

// getters/setters
void HttpResponse::setBody(const std::string &path)
{
    m_body = resourceToStr(path);
}

void HttpResponse::setStatusLine(void)
{
    auto it = m_statusCodes.find(m_statusCode);
    if (it == m_statusCodes.end())
        m_statusLine = "HTTP/1.1 500 Internal Server Error";
    else
        m_statusLine = it->second;
}

// methods
std::string HttpResponse::responseBuild(void)
{
    setStatusLine();
    std::string httpResponse = m_statusLine + "\r\n";
    for (const auto &pair : m_headers)
    {
        httpResponse += pair.first + ": " + pair.second + "\r\n";
    }
    httpResponse += "\r\n";
    httpResponse += m_body;

    return httpResponse;
}

int HttpResponse::targetRead(const std::string &requestTarget)
{
    int n{};
    std::ifstream inf{requestTarget};
    if (!inf)
        throw std::runtime_error("404 NOT FOUND\n");
    while (inf)
    {
        std::string strInput;
        std::getline(inf, strInput);
        m_body.append(strInput);
        n = n + strInput.length();
    }

    return n;
}

/*
deze functie moet beter
/index kan opgevraagd worden
/directories kunnen ook opgevraagd worden
/ kan ook opgevraagd worden
moet minsten een / zijn
...
*/
std::string HttpResponse::targetPathCreate(const std::string &target)
{
    if (!target.compare("/"))
        return "./www/index.html";

    return "./www" + target + ".html";
}

std::string HttpResponse::resourceToStr(const std::string &path)
{
    std::ifstream inf(path);
    if (!inf)
        throw HttpStatusCodeException(404);
    std::ostringstream sstr;
    sstr << inf.rdbuf();
    return sstr.str();
}

std::string generateHtmlString(const std::string &path)
{
    std::stringstream htmlStream;
    std::string folderPath = "." + path;

    htmlStream << "<html><head><title>File and Directory List</title></head><body>\n";
    htmlStream << "<h1>Files and Directories in " << folderPath << "</h1>\n";
    htmlStream << "<ul>\n";

    for (const auto &entry : std::filesystem::directory_iterator(folderPath))
    {
        const std::string entryName = entry.path().filename().string();
        if (std::filesystem::is_directory(entry))
        {
            htmlStream << "<li><strong>Directory:</strong> <a href=\"" << path + "/" + entryName << "\">" << entryName << "</a></li>\n";
        }
        else if (std::filesystem::is_regular_file(entry))
        {
            htmlStream << "<li><strong>File:</strong> " << entryName << " ";
            htmlStream << "<button onclick=\"deleteFile('" << path + "/" + entryName << "')\">Delete</button></li>\n";
        }
    }

    htmlStream << "</ul>\n";
    htmlStream << "<script>\n";
    htmlStream << "function deleteFile(fileName) {\n";
    htmlStream << "    if (confirm('Are you sure you want to delete ' + fileName + '?')) {\n";
    htmlStream << "        var xhr = new XMLHttpRequest();\n";
    htmlStream << "        xhr.open('DELETE', fileName, true);\n";
    htmlStream << "        xhr.onreadystatechange = function() {\n";
    htmlStream << "            if (xhr.readyState === 4 && xhr.status === 200) {\n";
    htmlStream << "                alert('File ' + fileName + ' deleted.');\n";
    htmlStream << "                // Refresh the page or update the file list as needed\n";
    htmlStream << "            }\n";
    htmlStream << "        };\n";
    htmlStream << "        xhr.send();\n";
    htmlStream << "    }\n";
    htmlStream << "}\n";
    htmlStream << "</script>\n";
    htmlStream << "</body></html>\n";

    return htmlStream.str();
}

bool isDirectory(std::string path)
{
    if (std::filesystem::is_directory(path))
        return true;
    return false;
}

void HttpResponse::getHandle(void)
{
    std::string path{m_request.m_methodPathVersion[1]};
    if (!path.compare("/"))
    {
        path = "./www/index.html";
        setBody(path);
    }
    else if (path.find("/cgi-bin/") != std::string::npos)
    {
        CGI CGI(path);
        // what does child process do when error thrown?
        CGI.execute();
        m_body = std::string(CGI.m_readBuf);
    }
    else if (isDirectory("." + path))
    {
        spdlog::debug("PATH IS DIR");
        m_body = generateHtmlString(path);
    }
    else
    {
        path = "./www" + m_request.m_methodPathVersion[1] + ".html";
        setBody(path);
    }
}

void HttpResponse::postHandle(void)
{
    if (m_request.m_methodPathVersion[1].find("/cgi-bin/") != std::string::npos)
    {
        CGI CGI(m_request.m_methodPathVersion[1], "num1=5&num2=5");
        // set correct body for cgi parameters
        CGI.execute();
        m_body = std::string(CGI.m_readBuf);
    }
}

void HttpResponse::deleteHandle(void)
{
    std::string path{m_request.m_methodPathVersion[1]};
    std::string allowedPath = "/www/files";

    if (path.compare(0, allowedPath.length(), allowedPath) != 0)
        throw HttpStatusCodeException(410);
    else if (std::remove("./www/hello.txt") == 0)
        m_body = "Succes";
    else
        throw HttpStatusCodeException(411);
}

void HttpResponse::responseHandle(void)
{
    try
    {
        if (!m_request.m_methodPathVersion[0].compare("GET"))
        {
            spdlog::debug("GET method");
            getHandle();
        }
        else if (!m_request.m_methodPathVersion[0].compare("POST"))
        {
            spdlog::info("POST method");
            postHandle();
        }
        else if (!m_request.m_methodPathVersion[0].compare("DELETE"))
        {
            spdlog::debug("DELETE method");
            deleteHandle();
        }
    }
    catch (const HttpStatusCodeException &e)
    {
        m_statusCode = e.getErrorCode();
    }
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const HttpResponse &httpresponse)
{
    out << "HttpResponse(\n";
    out << httpresponse.m_statusCode << '\n';
    out << httpresponse.m_statusLine << '\n';
    out << ")";

    return out;
}