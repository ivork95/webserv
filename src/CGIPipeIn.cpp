#include "CGIPipeIn.hpp"

CGIPipeIn::CGIPipeIn(Client &client) : m_client(client)
{
}

void CGIPipeIn::dupCloseWrite(std::vector<Socket *> &toBeDeleted)
{
    if (dup2(m_pipeFd[READ], STDIN_FILENO) == -1) // Dup the READ end of pipe1 to stdin
    {
        close(m_pipeFd[READ]);
        m_pipeFd[READ] = -1;
        close(m_pipeFd[WRITE]);
        m_pipeFd[WRITE] = -1;
        toBeDeleted.push_back(this);
        throw StatusCodeException(500, "Error: dup2()");
    }

    if (close(m_pipeFd[READ]) == -1)
    {
        m_pipeFd[READ] = -1;
        close(m_pipeFd[WRITE]);
        m_pipeFd[WRITE] = -1;
        toBeDeleted.push_back(this);
        throw StatusCodeException(500, "Error: close()");
    }
    m_pipeFd[READ] = -1;

    int nbytes{static_cast<int>(write(m_pipeFd[WRITE], m_input, std::strlen(m_input)))}; // Write to stdin
    if (nbytes == -1)
    {
        close(m_pipeFd[WRITE]);
        m_pipeFd[WRITE] = -1;
        toBeDeleted.push_back(this);
        throw StatusCodeException(500, "Error: write()");
    }

    if (close(m_pipeFd[WRITE]) == -1)
    {
        m_pipeFd[WRITE] = -1;
        toBeDeleted.push_back(this);
        throw StatusCodeException(500, "Error: close()");
    }
    m_pipeFd[WRITE] = -1;
}