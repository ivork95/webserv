#include "CGIPipeOut.hpp"

CGIPipeOut::CGIPipeOut(Client &client, Request &request, Response &response) : m_client(client), m_request(request), m_response(response)
{
    ;
}

void CGIPipeOut::forkDupAndExec(void)
{
    char *pythonPath = "/usr/bin/python3"; // Path to the Python interpreter
    auto elem = m_request.m_locationconfig.getCgiHandler().begin();
    const char *pythonPath2 = elem->second.c_str();

    spdlog::critical("python path: {}", pythonPath2);
    char *scriptPath = "./hello.py";       // Path to the Python script
    char *argv[] = {pythonPath, scriptPath, NULL};
    char *env[] = {"ARG1=hello", "ARG2=bye", NULL};
    pid_t cpid = fork();
    if (cpid == -1)
        throw StatusCodeException(500, "Error: fork())");
    if (cpid == 0)
    {
        if (close(m_pipeFd[READ]) == -1)
            throw StatusCodeException(500, "Error: close())");
        if (dup2(m_pipeFd[WRITE], STDOUT_FILENO) == -1) // Dup the write end of pipe2 to stdout
            throw StatusCodeException(500, "Error: dup2() 1");
        if (close(m_pipeFd[WRITE]) == -1)
            throw StatusCodeException(500, "Error: close()");
        execve(pythonPath, argv, env);
        throw StatusCodeException(500, "Error: execve()");
    }
    else
    {
        int wstatus{};
        if (wait(&wstatus) == -1)
            throw StatusCodeException(500, "Error: wait()");
        if (wstatus)
            throw StatusCodeException(500, "Error: wstatus");
        if (close(m_pipeFd[WRITE]) == -1)
            throw StatusCodeException(500, "Error: close()");
    }
}