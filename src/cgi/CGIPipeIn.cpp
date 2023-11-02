#include "CGIPipeIn.hpp"

CGIPipeIn::CGIPipeIn(Client &client) : m_client(client)
{
    if (pipe(m_pipeFd) == -1)
        throw StatusCodeException(500, "Error: pipe()");
    if (Helper::setNonBlocking(m_pipeFd[READ]) == -1)
        throw StatusCodeException(500, "Error: fcntl()");
    if (Helper::setNonBlocking(m_pipeFd[WRITE]) == -1)
        throw StatusCodeException(500, "Error: fcntl()");
}

void CGIPipeIn::dupCloseWrite(std::vector<Socket *> &toBeDeleted)
{
    if (dup2(m_pipeFd[READ], STDIN_FILENO) == -1) // Dup the READ end of pipe1 to stdin
    {
        close(m_pipeFd[READ]);
        close(m_pipeFd[WRITE]);
        m_socketFd = -1;
        throw StatusCodeException(500, "Error: dup2()");
    }

    if (close(m_pipeFd[READ]) == -1)
    {
        close(m_pipeFd[WRITE]);
        m_socketFd = -1;
        throw StatusCodeException(500, "Error: close()");
    }

    int nbytes{static_cast<int>(write(m_pipeFd[WRITE], m_client.m_request.m_body.c_str(), m_client.m_request.m_body.length()))}; // Write to stdin
    m_socketFd = -1;
    if (close(m_pipeFd[WRITE]) == -1)
        throw StatusCodeException(500, "Error: close()");
    if (nbytes == -1)
        throw StatusCodeException(500, "Error: write()");
}