#include "CGIPipeIn.hpp"
#include "Client.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
// constructor
CGIPipeIn::CGIPipeIn(Client &client) : m_client(client)
{
    if (pipe(m_pipeFd) == -1)
        throw StatusCodeException(500, "pipe()", errno);
    if (Helper::setNonBlocking(m_pipeFd[READ]) == -1)
        throw StatusCodeException(500, "fcntl()", errno);
    if (Helper::setNonBlocking(m_pipeFd[WRITE]) == -1)
        throw StatusCodeException(500, "fcntl()", errno);
    m_socketFd = m_pipeFd[WRITE];
}

CGIPipeIn::~CGIPipeIn(void)
{
    if (m_pipeFd[READ] != -1)
        close(m_pipeFd[READ]);
    if (m_pipeFd[WRITE] != -1)
        close(m_pipeFd[WRITE]);

    spdlog::debug("{} destructor", *this);
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const CGIPipeIn &cgipipein)
{
    out << "CGIPipeIn(" << cgipipein.m_pipeFd[READ] << ": " << cgipipein.m_pipeFd[WRITE] << ")";

    return out;
}

// member functions
void CGIPipeIn::dupCloseWrite(void)
{
    if (dup2(m_pipeFd[READ], STDIN_FILENO) == -1)
        throw StatusCodeException(500, "dup2()", errno);

    if (close(m_pipeFd[READ]) == -1)
        throw StatusCodeException(500, "close()", errno);
    m_pipeFd[READ] = -1;

    int nbytes{static_cast<int>(write(m_pipeFd[WRITE], m_client.getRequest().getBody().c_str(), m_client.getRequest().getBody().length()))}; // Write to stdin
    if (close(m_pipeFd[WRITE]) == -1)
        throw StatusCodeException(500, "close()", errno);
    m_pipeFd[WRITE] = -1;
    if (nbytes == -1)
        throw StatusCodeException(500, "write()", errno);
}
