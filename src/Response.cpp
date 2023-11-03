#include "Response.hpp"
#include <sys/types.h>
#include <sys/socket.h>

// constructor
Response::Response(void)
{
    // spdlog::debug("Response constructor called");
    Logger::getInstance().debug("Response constructor called");
}

// copy constructor

// copy assignment operator overload

// destructor
Response::~Response(void)
{
    // spdlog::debug("Response destructor called");
    Logger::getInstance().debug("Response destructor called");
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const Response &response)
{
    out << "Response {\n";
    out << "m_statusCode = |" << response.m_statusCode << "|\n";
    out << "m_statusLine = |" << response.m_statusLine << "|\n";
    out << "m_path = |" << response.m_path << "|\n";
    out << "}";

    return out;
}

// getters/setters
void Response::bodySet(const std::string &path)
{
    m_body = Helper::fileToStr(path);
}

void Response::statusLineSet(void)
{
    auto it = m_statusCodes.find(m_statusCode);
    if (it == m_statusCodes.end())
        m_statusLine = "HTTP/1.1 500 Internal Server Error";
    else
        m_statusLine = it->second;
}

// methods
std::string Response::responseBuild(void)
{
    statusLineSet();
    std::string httpResponse = m_statusLine + "\r\n";
    for (const auto &pair : m_headers)
        httpResponse += pair.first + ": " + pair.second + "\r\n";
    httpResponse += "\r\n";
    httpResponse += m_body;

    return httpResponse;
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
    htmlStream << "        var xhr = new XMLRequest();\n";
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

int sendAll(int sockFd, char *buf, int *len, int *total, int *bytesleft)
{
    if (*total < *len)
    {
        int nbytes{static_cast<int>(send(sockFd, buf + *total, *bytesleft, 0))};
        if (nbytes == -1)
            return -1;
        *total += nbytes;
        *bytesleft -= nbytes;
    }
    if (*bytesleft)
        return 1;

    return 0;
}

int Response::sendAll(int sockFd)
{
    if (m_buf.empty())
    {
        m_buf = responseBuild();
        m_len = m_buf.size();
        m_bytesleft = m_len;
    }
    if (m_total < m_len)
    {
        int nbytes{static_cast<int>(send(sockFd, m_buf.data() + m_total, m_bytesleft, 0))};
        if (nbytes == -1)
            return -1;
        m_total += nbytes;
        m_bytesleft -= nbytes;
    }
    if (m_bytesleft)
        return 1;

    return 0;
}