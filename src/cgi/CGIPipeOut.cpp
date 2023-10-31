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

void CGIPipeOut::forkCloseDupExec(std::vector<Socket *> &toBeDeleted)
{
    pid_t cpid1 = fork();
    if (cpid1 == -1)
    {
        close(m_pipeFd[READ]);
        m_pipeFd[READ] = -1;
        close(m_pipeFd[WRITE]);
        m_pipeFd[WRITE] = -1;
        toBeDeleted.push_back(this);
        throw StatusCodeException(500, "Error: fork())");
    }
    if (cpid1 == 0)
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

    pid_t cpid2 = fork();
    if (cpid2 == 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        _exit(0);
    }

    int wstatus{};
    pid_t wpid{};
    while (true)
    {
        wpid = waitpid(0, &wstatus, WNOHANG);
        if (wpid == cpid1)
        {
            spdlog::debug("cpid: {}: completed!", cpid1);

            kill(cpid2, SIGKILL);

            // if (wstatus)
            // {
            //     close(m_pipeFd[READ]);
            //     m_pipeFd[READ] = -1;
            //     close(m_pipeFd[WRITE]);
            //     m_pipeFd[WRITE] = -1;
            //     toBeDeleted.push_back(this);
            //     throw StatusCodeException(500, "Error: wstatus");
            // }

            // if (close(m_pipeFd[WRITE]) == -1)
            // {
            //     close(m_pipeFd[READ]);
            //     m_pipeFd[READ] = -1;
            //     m_pipeFd[WRITE] = -1;
            //     toBeDeleted.push_back(this);
            //     throw StatusCodeException(500, "Error: close()");
            // }
            // m_pipeFd[WRITE] = -1;
            break;
        }
        else if (wpid == cpid2)
        {
            spdlog::debug("cpid: {}: completed!", cpid2);

            kill(cpid1, SIGKILL);

            close(m_pipeFd[READ]);
            m_pipeFd[READ] = -1;
            close(m_pipeFd[WRITE]);
            m_pipeFd[WRITE] = -1;
            toBeDeleted.push_back(this);
            throw StatusCodeException(504, "Error: killed execve for taking to long");
        }
        else if (wpid == -1)
        {
            spdlog::debug("Error: waitpid()", cpid2);
            break;
        }
    }
}