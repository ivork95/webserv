#include "CGIPipeOut.hpp"
#include <thread>
#include <unistd.h>
#include <iostream>

CGIPipeOut::CGIPipeOut(Client &client, Request &request, Response &response) : m_client(client), m_request(request), m_response(response)
{
    if (pipe(m_pipeFd) == -1)
        throw StatusCodeException(500, "Error: pipe()");
    if (Helper::setNonBlocking(m_pipeFd[READ]) == -1)
        throw StatusCodeException(500, "Error: setNonBlocking()");
    if (Helper::setNonBlocking(m_pipeFd[WRITE]) == -1)
        throw StatusCodeException(500, "Error: setNonBlocking()");
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const CGIPipeOut &cgipipeout)
{
    out << "cgipipeout(" << cgipipeout.m_socketFd << cgipipeout.m_pipeFd[READ] << ": " << cgipipeout.m_pipeFd[WRITE] << ")";

    return out;
}

// void CGIPipeOut::forkCloseDupExec(std::vector<Socket *> &toBeDeleted)
// {
//     pid_t cpid = fork();
//     if (cpid == -1)
//     {
//         close(m_pipeFd[READ]);
//         close(m_pipeFd[WRITE]);
//         m_socketFd = -1;
//         toBeDeleted.push_back(this);
//         throw StatusCodeException(500, "Error: fork()");
//     }
//     if (cpid == 0)
//     {
//         char *pythonPath = strdup(m_request.m_locationconfig.getCgiInterpreter().c_str());
//         if (!pythonPath)
//             throw StatusCodeException(500, "Error: strdup)");

//         char *scriptPath = strdup(m_request.m_locationconfig.getAbsCgiScript().c_str());
//         if (!scriptPath)
//         {
//             delete[] pythonPath;
//             throw StatusCodeException(500, "Error: strdup)");
//         }

//         char *argv[] = {pythonPath, scriptPath, NULL};
//         char *env[] = {"ARG1=hello", NULL};
//         if (close(m_pipeFd[READ]) == -1)
//             throw StatusCodeException(500, "Error: close())");
//         if (dup2(m_pipeFd[WRITE], STDOUT_FILENO) == -1) // Dup the write end of pipe2 to stdout
//             throw StatusCodeException(500, "Error: dup2() 1");
//         if (close(m_pipeFd[WRITE]) == -1)
//             throw StatusCodeException(500, "Error: close()");
//         execve(pythonPath, argv, env);
//         delete[] pythonPath;
//         delete[] scriptPath;
//         throw StatusCodeException(500, "Error: execve()");
//     }
//     else
//     {
//         int wstatus{};
//         if ((waitpid(cpid, &wstatus, 0) == -1) || wstatus)
//         {
//             close(m_pipeFd[READ]);
//             close(m_pipeFd[WRITE]);
//             m_socketFd = -1;
//             toBeDeleted.push_back(this);
//             throw StatusCodeException(502, "Error: wait() || execve exception");
//         }
//         if (close(m_pipeFd[WRITE]) == -1)
//         {
//             close(m_pipeFd[READ]);
//             m_socketFd = -1;
//             toBeDeleted.push_back(this);
//             throw StatusCodeException(500, "Error: close()");
//         }
//     }
// }

void CGIPipeOut::forkCloseDupExec(std::vector<Socket *> &toBeDeleted)
{
	Logger &logger = Logger::getInstance();

    pid_t cpid1 = fork();
    if (cpid1 == -1)
    {
        close(m_pipeFd[READ]);
        close(m_pipeFd[WRITE]);
        m_socketFd = -1;
        toBeDeleted.push_back(this);
        throw StatusCodeException(500, "Error: fork()");
    }
    if (cpid1 == 0)
    {
        char *pythonPath = "/usr/bin/python3";     // Path to the Python interpreter
        char *scriptPath = "www/cgi-bin/hello.py"; // Path to the Python script
        char *argv[] = {pythonPath, scriptPath, NULL};

        if (close(m_pipeFd[READ]) == -1)
            throw StatusCodeException(500, "Error: close()");
        if (dup2(m_pipeFd[WRITE], STDOUT_FILENO) == -1) // Dup the write end of pipe2 to stdout
            throw StatusCodeException(500, "Error: dup2()");
        if (close(m_pipeFd[WRITE]) == -1)
            throw StatusCodeException(500, "Error: close()");
        execve(pythonPath, argv, NULL);
        throw StatusCodeException(500, "Error: execve()");
    }

    pid_t cpid2 = fork();
    if (cpid2 == -1)
        throw StatusCodeException(500, "Error: fork()");
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
            // spdlog::debug("execve completed!");
			logger.debug("execve completed!");

            kill(cpid2, SIGKILL);

            if (wstatus)
            {
                // spdlog::warn("execve exited with exception: {}", cpid1, wstatus);
				logger.debug("execve exited with exception: " + std::to_string(cpid1) + " " + std::to_string(wstatus));

                close(m_pipeFd[READ]);
                close(m_pipeFd[WRITE]);
                m_socketFd = -1;
                toBeDeleted.push_back(this);
                throw StatusCodeException(502, "Error: excevce exception");
            }

            if (close(m_pipeFd[WRITE]) == -1)
            {
                close(m_pipeFd[READ]);
                m_socketFd = -1;
                toBeDeleted.push_back(this);
                throw StatusCodeException(500, "Error: close()");
            }
            break;
        }
        else if (wpid == cpid2)
        {
            // spdlog::debug("cpid2 completed!");
			logger.debug("cpid2 completed!");

            kill(cpid1, SIGKILL);

            close(m_pipeFd[READ]);
            close(m_pipeFd[WRITE]);
            m_socketFd = -1;
            toBeDeleted.push_back(this);
            throw StatusCodeException(504, "Error: killed execve for taking to long");
        }
        else if (wpid == -1)
            throw StatusCodeException(502, "Error: waitpid()");
    }
}