#include "CGIPipeOut.hpp"
#include "Request.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

CGIPipeOut::CGIPipeOut(Client &client, Request &request, Response &response) : m_client(client), m_request(request), m_response(response)
{
    if (pipe(m_pipeFd) == -1)
        throw StatusCodeException(500, "pipe()", errno);
    if (Helper::setNonBlocking(m_pipeFd[READ]) == -1)
        throw StatusCodeException(500, "setNonBlocking()", errno);
    if (Helper::setNonBlocking(m_pipeFd[WRITE]) == -1)
        throw StatusCodeException(500, "setNonBlocking()", errno);
    
    m_socketFd = m_pipeFd[READ];
    spdlog::critical("CGIpipeOut: {}", *this);
}

CGIPipeOut::~CGIPipeOut(void)
{
    if (m_pipeFd[READ] != -1)
        close(m_pipeFd[READ]);
    if (m_pipeFd[WRITE] != -1)
        close(m_pipeFd[WRITE]);
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const CGIPipeOut &cgipipeout)
{
    out << "READ = " << cgipipeout.m_pipeFd[READ] << " | WRITE = " << cgipipeout.m_pipeFd[WRITE] << "\nsocketFD = " << cgipipeout.m_socketFd << "\n";
    return out;
}

// std::ostream &operator<<(std::ostream &out, const CGIPipeOut &cgipipeout)
// {
//     out << "cgipipeout(" << cgipipeout.m_socketFd << cgipipeout.m_pipeFd[READ] << ": " << cgipipeout.m_pipeFd[WRITE] << ")";

//     return out;
// }

void CGIPipeOut::forkCloseDupExec(void)
{
    pid_t cpid1 = fork();
    if (cpid1 == -1)
        throw StatusCodeException(500, "fork()", errno);
    if (cpid1 == 0)
    {
        char *pythonPath = strdup(m_request.m_locationconfig.getCgiInterpreter().c_str());
        if (pythonPath == NULL)
            throw StatusCodeException(500, "Error: strdup)");
        char *scriptPath = strdup(m_request.m_locationconfig.getAbsCgiScript().c_str());
        if (scriptPath == NULL)
        {
            delete[] pythonPath;
            throw StatusCodeException(500, "Error: strdup)");
        }
        char *argv[] = {pythonPath, scriptPath, NULL};
        char **env = NULL;
        if (!m_request.m_query.empty())
        {
            size_t i = m_request.m_query.size();
            env = new char*[i + 1];
            env[i] = NULL;
            i = 0;
            for (const auto &query: m_request.m_query)
            {
                env[i] = strdup(query.c_str());
                if (scriptPath == NULL)
                {
                    delete[] pythonPath;
                    delete[] scriptPath;
                    throw StatusCodeException(500, "Error: strdup)");
                }
                i++;
            }
        }
        if (close(m_pipeFd[READ]) == -1)
            throw StatusCodeException(500, "Error: close())");
        if (dup2(m_pipeFd[WRITE], STDOUT_FILENO) == -1) // Dup the write end of pipe2 to stdout
            throw StatusCodeException(500, "Error: dup2() 1");
        if (close(m_pipeFd[WRITE]) == -1)
            throw StatusCodeException(500, "Error: close()");
        execve(pythonPath, argv, env);
        delete[] pythonPath;
        delete[] scriptPath;
        for (size_t i = 0; i < m_request.m_query.size(); i++)
            free(env[i]);
        delete[] env;
        throw StatusCodeException(500, "Error: execve()");
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
            kill(cpid2, SIGKILL);
            if (wstatus)
                throw StatusCodeException(502, "execve", errno);
            if (close(m_pipeFd[WRITE]) == -1)
                throw StatusCodeException(500, "close()", errno);
            m_pipeFd[WRITE] = -1;
            break;
        }
        else if (wpid == cpid2)
        {
            kill(cpid1, SIGKILL);
            throw StatusCodeException(504, "Killed execve for taking too long");
        }
        else if (wpid == -1)
            throw StatusCodeException(502, "waitpid()", errno);
    }
}