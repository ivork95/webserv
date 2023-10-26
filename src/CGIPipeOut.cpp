#include "CGIPipeOut.hpp"

CGIPipeOut::CGIPipeOut(Client &client, Request &request, Response &response) : m_client(client), m_request(request), m_response(response)
{
}

void CGIPipeOut::forkCloseDupExec(std::vector<Socket *> &toBeDeleted)
{
    pid_t cpid = fork();
    if (cpid == -1)
    {
        close(m_pipeFd[READ]);
        m_pipeFd[READ] = -1;
        close(m_pipeFd[WRITE]);
        m_pipeFd[WRITE] = -1;
        toBeDeleted.push_back(this);
        throw StatusCodeException(500, "Error: fork())");
    }
    if (cpid == 0)
    {
        char *pythonPath = "/usr/bin/python3";     // Path to the Python interpreter
        char *scriptPath = "www/cgi-bin/hello.py"; // Path to the Python script
        char *argv[] = {pythonPath, scriptPath, NULL};

        if (close(m_pipeFd[READ]) == -1)
            throw StatusCodeException(500, "Error: close())");
        if (dup2(m_pipeFd[WRITE], STDOUT_FILENO) == -1) // Dup the write end of pipe2 to stdout
            throw StatusCodeException(500, "Error: dup2() 1");
        if (close(m_pipeFd[WRITE]) == -1)
            throw StatusCodeException(500, "Error: close()");
        execve(pythonPath, argv, NULL);
        throw StatusCodeException(500, "Error: execve()");
    }
    else
    {
        int wstatus{};
        if (wait(&wstatus) == -1)
        {
            close(m_pipeFd[READ]);
            m_pipeFd[READ] = -1;
            close(m_pipeFd[WRITE]);
            m_pipeFd[WRITE] = -1;
            toBeDeleted.push_back(this);
            throw StatusCodeException(500, "Error: wait()");
        }
        if (wstatus)
        {
            close(m_pipeFd[READ]);
            m_pipeFd[READ] = -1;
            close(m_pipeFd[WRITE]);
            m_pipeFd[WRITE] = -1;
            toBeDeleted.push_back(this);
            throw StatusCodeException(500, "Error: wstatus");
        }
        if (close(m_pipeFd[WRITE]) == -1)
        {
            close(m_pipeFd[READ]);
            m_pipeFd[READ] = -1;
            m_pipeFd[WRITE] = -1;
            toBeDeleted.push_back(this);
            throw StatusCodeException(500, "Error: close()");
        }
        m_pipeFd[WRITE] = -1;
    }
}