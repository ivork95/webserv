#include "CGIPipeIn.hpp"

// constructor
CGIPipeIn::CGIPipeIn(Client &client) : m_client(client)
{
    if (pipe(m_pipeFd) == -1)
        throw StatusCodeException(500, "pipe()", errno);
    if (Helper::setNonBlocking(m_pipeFd[READ]) == -1)
        throw StatusCodeException(500, "fcntl()", errno);
    if (Helper::setNonBlocking(m_pipeFd[WRITE]) == -1)
        throw StatusCodeException(500, "fcntl()", errno);
}

// member functions
void CGIPipeIn::dupCloseWrite(std::vector<ASocket *> &toBeDeleted)
{
    if (dup2(m_pipeFd[READ], STDIN_FILENO) == -1)
    {
        close(m_pipeFd[READ]);
        close(m_pipeFd[WRITE]);
        m_socketFd = -1;
        toBeDeleted.push_back(this);
        throw StatusCodeException(500, "dup2()", errno);
    }

    if (close(m_pipeFd[READ]) == -1)
    {
        close(m_pipeFd[WRITE]);
        m_socketFd = -1;
        toBeDeleted.push_back(this);
        throw StatusCodeException(500, "close()", errno);
    }

    int nbytes{static_cast<int>(write(m_pipeFd[WRITE], m_client.m_request.m_body.c_str(), m_client.m_request.m_body.length()))}; // Write to stdin
    m_socketFd = -1;
    if (close(m_pipeFd[WRITE]) == -1)
        throw StatusCodeException(500, "close()", errno);
    if (nbytes == -1)
        throw StatusCodeException(500, "write()", errno);
}