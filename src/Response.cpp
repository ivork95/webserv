#include "Response.hpp"
#include <sys/types.h>
#include <sys/socket.h>

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

void Response::statusLineSet(void)
{
    auto it = m_statusCodes.find(m_statusCode);
    if (it == m_statusCodes.end())
        m_statusLine = "HTTP/1.1 500 Internal Server Error";
    else
        m_statusLine = it->second;
}

// methods
std::string Response::responseBuild(std::vector<ErrorPageConfig> errorPages)
{
    for (const auto &errorPageConfig : errorPages)
    {
        for (const auto &errorCode : errorPageConfig.getErrorCodes())
        {
            if (errorCode == m_statusCode)
            {
                try // try catch in case error page doesnt exist. Is it possible to check all files during parsing?
                {
                    m_body = Helper::fileToStr(errorPageConfig.getFilePath());
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << '\n';
                }
            }
        }
    }
    statusLineSet();
    std::string httpResponse = m_statusLine + "\r\n";
    for (const auto &pair : m_headers)
        httpResponse += pair.first + ": " + pair.second + "\r\n";
    httpResponse += "\r\n";
    httpResponse += m_body;

    return httpResponse;
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

int Response::sendAll(int sockFd, std::vector<ErrorPageConfig> errorPages)
{
    if (m_buf.empty())
    {
        m_buf = responseBuild(errorPages);
        m_len = m_buf.size();
        m_bytesleft = m_len;
    }
    if (m_total < m_len)
    {
        int nbytes{static_cast<int>(send(sockFd, m_buf.data() + m_total, m_bytesleft, 0))};
        // std::cout << *this << std::endl;
        if (nbytes == -1)
            return -1;
        m_total += nbytes;
        m_bytesleft -= nbytes;
    }
    if (m_bytesleft)
        return 1;

    return 0;
}