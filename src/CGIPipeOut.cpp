#include "CGIPipeOut.hpp"
#include <unistd.h>

CGIPipeOut::CGIPipeOut(Client &client, Request &request, Response &response) : m_client(client), m_request(request), m_response(response)
{
    ;
}

void CGIPipeOut::forkDupAndExec(void)
{
    pid_t cpid = fork();
    if (cpid == -1)
        throw StatusCodeException(500, "Error: fork())");
    if (cpid == 0)
    {

        // std::map<std::string, std::string>::const_iterator elem = m_request.m_locationconfig.getCgiHandler().begin();
        // std::string temp{elem->second};
        // char* pythonPath = new char[elem->second.size() + 1];
		std::string tmpMerge{m_request.m_locationconfig.getCgiInterpreter()};
        char* pythonPath = new char[tmpMerge.size() + 1];
        strcpy(pythonPath, tmpMerge.c_str());

        // std::string scriptLocation = "./" + m_request.m_locationconfig.getRootPath() + m_request.m_locationconfig.getRequestURI() + "/hello" + elem->first;       // Path to the Python script
        // char* scriptPath = new char[scriptLocation.size() + 1];
		std::string tmpScript{"./" + m_request.m_locationconfig.getRootPath() + m_request.m_locationconfig.getRequestURI() + "/" + m_request.m_locationconfig.getCgiScript()};
        char* scriptPath = new char[tmpScript.size() + 1];
        strcpy(scriptPath, tmpScript.c_str());
        // if (!access (scriptPath, X_OK))
        //     throw StatusCodeException(500, "CGI script not executable");
        spdlog::critical("FULL script path: {}", scriptPath);
        char *argv[] = {pythonPath, scriptPath, NULL};
        char *env[] = {"ARG1=hello", "ARG2=bye", NULL};
        if (close(m_pipeFd[READ]) == -1)
            throw StatusCodeException(500, "Error: close())");
        if (dup2(m_pipeFd[WRITE], STDOUT_FILENO) == -1) // Dup the write end of pipe2 to stdout
            throw StatusCodeException(500, "Error: dup2() 1");
        if (close(m_pipeFd[WRITE]) == -1)
            throw StatusCodeException(500, "Error: close()");
        execve(pythonPath, argv, env);
        delete[] pythonPath;
        delete[] scriptPath;
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