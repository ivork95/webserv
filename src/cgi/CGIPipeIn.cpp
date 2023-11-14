#include "CGIPipeIn.hpp"
#include "Client.hpp"

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

CGIPipeIn::~CGIPipeIn(void)
{
    if (m_pipeFd[READ] != -1)
        close(m_pipeFd[READ]);
    if (m_pipeFd[WRITE] != -1)
        close(m_pipeFd[WRITE]);
}

// member functions
void CGIPipeIn::dupCloseWrite(void)
{
    if (dup2(m_pipeFd[READ], STDIN_FILENO) == -1)
        throw StatusCodeException(500, "dup2()", errno);

    if (close(m_pipeFd[READ]) == -1)
        throw StatusCodeException(500, "close()", errno);
    m_pipeFd[READ] = -1;

    // int nbytes{static_cast<int>(write(m_pipeFd[WRITE], m_client.m_request.m_body.c_str(), m_client.m_request.m_body.length()))}; // Write to stdin
    if (m_client.m_request.m_body.length() == 0)
    {
        Multiplexer &multiplexer = Multiplexer::getInstance();
        int nbytes{static_cast<int>(write(m_pipeFd[WRITE], "No input provided.", 18))}; // Write to stdin
        if (close(m_pipeFd[WRITE]) == -1)
            throw StatusCodeException(500, "close()", errno);
        m_pipeFd[WRITE] = -1;
        if (nbytes == -1)
            throw StatusCodeException(500, "write()", errno);
        return ;
    }
    int nbytes{static_cast<int>(write(m_pipeFd[WRITE], m_client.m_request.m_body.c_str(), m_client.m_request.m_body.length()))}; // Write to stdin
    if (close(m_pipeFd[WRITE]) == -1)
        throw StatusCodeException(500, "close()", errno);
    m_pipeFd[WRITE] = -1;
    if (nbytes == -1)
        throw StatusCodeException(500, "write()", errno);
}