#include "CGIPipeOut.hpp"

CGIPipeOut::CGIPipeOut(Client &client, Request &request, Response &response) : m_client(client), m_request(request), m_response(response)
{
    if (pipe(m_pipeFd) == -1)
        throw StatusCodeException(500, "pipe()", errno);
    if (Helper::setNonBlocking(m_pipeFd[READ]) == -1)
        throw StatusCodeException(500, "setNonBlocking()", errno);
    if (Helper::setNonBlocking(m_pipeFd[WRITE]) == -1)
        throw StatusCodeException(500, "setNonBlocking()", errno);
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const CGIPipeOut &cgipipeout)
{
    out << "cgipipeout(" << cgipipeout.m_socketFd << cgipipeout.m_pipeFd[READ] << ": " << cgipipeout.m_pipeFd[WRITE] << ")";

    return out;
}

void CGIPipeOut::forkCloseDupExec(std::vector<ASocket *> &toBeDeleted)
{
    pid_t cpid1 = fork();
    if (cpid1 == -1)
    {
        close(m_pipeFd[READ]);
        close(m_pipeFd[WRITE]);
        m_socketFd = -1;
        toBeDeleted.push_back(this);
        throw StatusCodeException(500, "fork()", errno);
    }
    if (cpid1 == 0)
    {
        char *pythonPath = "/usr/bin/python3";     // Path to the Python interpreter
        char *scriptPath = "www/cgi-bin/hello.py"; // Path to the Python script
        char *argv[] = {pythonPath, scriptPath, NULL};

        if (close(m_pipeFd[READ]) == -1)
            throw StatusCodeException(500, "close()", errno);
        if (dup2(m_pipeFd[WRITE], STDOUT_FILENO) == -1) // Dup the write end of pipe2 to stdout
            throw StatusCodeException(500, "dup2()", errno);
        if (close(m_pipeFd[WRITE]) == -1)
            throw StatusCodeException(500, "close()", errno);
        execve(pythonPath, argv, NULL);
        throw StatusCodeException(500, "execve()", errno);
    }

    pid_t cpid2 = fork();
    if (cpid2 == -1)
        throw StatusCodeException(500, "fork()", errno);
    if (cpid2 == 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        _exit(0);
    }

    int wstatus{};
    pid_t wpid{};
    while (true)
    {
        wpid = waitpid(-1, &wstatus, WNOHANG);
        if (wpid == cpid1)
        {
            std::cout << "execve completed\n";

            kill(cpid2, SIGKILL);
            if (wstatus)
            {
                std::cout << "execve exited with exception: " << wstatus << "\n";

                close(m_pipeFd[READ]);
                close(m_pipeFd[WRITE]);
                m_socketFd = -1;
                toBeDeleted.push_back(this);
                throw StatusCodeException(502, "execve", errno);
            }
            if (close(m_pipeFd[WRITE]) == -1)
            {
                close(m_pipeFd[READ]);
                m_socketFd = -1;
                toBeDeleted.push_back(this);
                throw StatusCodeException(500, "close()", errno);
            }
            break;
        }
        else if (wpid == cpid2)
        {
            std::cout << "cpid2 completed\n";

            kill(cpid1, SIGKILL);
            close(m_pipeFd[READ]);
            close(m_pipeFd[WRITE]);
            m_socketFd = -1;
            toBeDeleted.push_back(this);
            throw StatusCodeException(504, "Killed execve for taking too long");
        }
        else if (wpid == -1)
            throw StatusCodeException(502, "waitpid()", errno);
    }
}