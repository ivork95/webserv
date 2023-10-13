#include "HttpResponse.hpp"
#include <sys/types.h>
#include <sys/socket.h>

// request constructor
HttpResponse::HttpResponse(void)
{
    spdlog::debug("HttpResponse constructor called");
}

// copy constructor

// copy assignment operator overload

// destructor
HttpResponse::~HttpResponse(void)
{
    spdlog::debug("HttpResponse destructor called");
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const HttpResponse &httpresponse)
{
    out << "HttpResponse {\n";
    out << "m_statusCode = |" << httpresponse.m_statusCode << "|\n";
    out << "m_statusLine = |" << httpresponse.m_statusLine << "|\n";
    out << "m_path = |" << httpresponse.m_path << "|\n";
    out << "}";

    return out;
}

// getters/setters
void HttpResponse::bodySet(const std::string &path)
{
    m_body = Helper::fileToStr(path);
}

void HttpResponse::statusLineSet(void)
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
    statusLineSet();
    std::string httpResponse = m_statusLine + "\r\n";
    for (const auto &pair : m_headers)
        httpResponse += pair.first + ": " + pair.second + "\r\n";
    httpResponse += "\r\n";
    httpResponse += m_body;

    return httpResponse;
}

int sendall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while (total < *len)
    {
        n = send(s, buf + total, bytesleft, 0);
        if (n == -1)
        {
            break;
        }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
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
    // spdlog::critical("m_serverconfig = {}", m_serverconfig);

    // if (m_request.m_methodPathVersion[1] == m_request.m_locationconfig.getRequestURI())
    // {
    //     spdlog::critical("_requestURI = {}", m_request.m_locationconfig.getRequestURI());
    //     bool isIndexFileFound{false};
    //     for (auto &b : m_request.m_locationconfig.getIndexFile()) // loop over index files
    //     {
    //         spdlog::critical("actual path = {}", m_request.m_locationconfig.getRootPath() + b);
    //         try
    //         {
    //             bodySet(m_request.m_locationconfig.getRootPath() + b);
    //             m_path = m_request.m_locationconfig.getRootPath() + b;
    //             isIndexFileFound = true;
    //             break;
    //         }
    //         catch (const StatusCodeException &e)
    //         {
    //             m_path = m_request.m_locationconfig.getRootPath() + b;
    //             spdlog::warn(e.what());
    //         }
    //     }
    //     if (!isIndexFileFound)
    //         throw StatusCodeException(404, "Error: ifstream3");
    // }
    // m_statusCode = 200;
}

void HttpResponse::postHandle(void)
{
    // if (m_request.m_fileName.empty())
    //     throw StatusCodeException(400, "Error: no fileName");
    // bodyToDisk("./www/" + m_request.m_fileName);
    // m_headers.insert({"Location", "/" + percentEncode(m_request.m_fileName)});
    // m_statusCode = 303;
}

void HttpResponse::deleteHandle(void)
{
    // std::string path{m_request.m_methodPathVersion[1]};
    // std::string allowedPath = "/www/files";

    // if (path.compare(0, allowedPath.length(), allowedPath) != 0)
    //     throw StatusCodeException(410, "Error: compare()");
    // else if (std::remove("./www/hello.txt") == 0)
    //     m_body = "Succes";
    // else
    //     throw StatusCodeException(411, "Error: remove()");
}

void HttpResponse::responseHandle(void)
{
    // if (m_statusCode)
    //     return;

    // try
    // {
    //     if (m_request.m_methodPathVersion[0] == "GET")
    //     {
    //         spdlog::debug("GET method");
    //         getHandle();
    //     }
    //     else if (m_request.m_methodPathVersion[0] == "POST")
    //     {
    //         spdlog::info("POST method");
    //         postHandle();
    //     }
    //     else if (m_request.m_methodPathVersion[0] == "DELETE")
    //     {
    //         spdlog::debug("DELETE method");
    //         deleteHandle();
    //     }
    // }
    // catch (const StatusCodeException &e)
    // {
    //     m_statusCode = e.getStatusCode();
    //     spdlog::warn(e.what());
    // }
}