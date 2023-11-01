#include "CGIPipeOut.hpp"
#include <thread>

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
	Logger &logger = Logger::getInstance();

    pid_t cpid1{};
    pid_t cpid2{};
    int wstatus{};

    cpid1 = fork();
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

    cpid2 = fork();
    if (cpid2 == 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        kill(cpid1, SIGKILL);
        _exit(0);
    }

    waitpid(cpid1, &wstatus, 0);
    if (WIFSIGNALED(wstatus))
    {
        if (WTERMSIG(wstatus) == 9)
        {
            // spdlog::info("cpid2: {} killed cpid1: {}", cpid2, cpid1);
			logger.debug("cpid2: " + std::to_string(cpid2) + " killed cpid1: " + std::to_string(cpid1));

            close(m_pipeFd[READ]);
            m_pipeFd[READ] = -1;
            close(m_pipeFd[WRITE]);
            m_pipeFd[WRITE] = -1;
            toBeDeleted.push_back(this);
            throw StatusCodeException(504, "Error: execve took to long");
        }
        else
        {
            // spdlog::info("Execve exited with signal: {}", wstatus);
			logger.debug("Execve exited with signal: " + std::to_string(wstatus));

            kill(cpid2, SIGKILL);
            close(m_pipeFd[READ]);
            m_pipeFd[READ] = -1;
            close(m_pipeFd[WRITE]);
            m_pipeFd[WRITE] = -1;
            toBeDeleted.push_back(this);
            throw StatusCodeException(502, "Error: execve exited with error");
        }
    }
    if (WIFEXITED(wstatus))
    {
        // spdlog::info("Execve terminated normally");
		logger.debug("Execve terminated normally");
        kill(cpid2, SIGKILL);
    }
}