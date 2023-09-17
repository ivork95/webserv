#include "Cgi.hpp"
#include "StatusCodes.hpp"
#include <unistd.h>

CGI::CGI(std::string path)
{
	m_originalPath = path;
	size_t posQM = path.find("?");
	m_executable = "www" + path.substr(0, posQM);
	std::string paramString = path.substr(posQM + 1);
    
	size_t startPos = 0;
    while (startPos < paramString.length()) {
        size_t equalsPos = paramString.find('=', startPos);
        size_t ampersandPos = paramString.find('&', startPos);

        if (equalsPos != std::string::npos && ampersandPos != std::string::npos)
		{
            std::string paramName = paramString.substr(startPos, equalsPos - startPos);
            std::string paramValue = paramString.substr(equalsPos + 1, ampersandPos - equalsPos - 1);
            m_params.push_back(paramValue);
            startPos = ampersandPos + 1;
        }
		else if (equalsPos != std::string::npos)
		{
            std::string paramName = paramString.substr(startPos, equalsPos - startPos);
            std::string paramValue = paramString.substr(equalsPos + 1);
            m_params.push_back(paramValue);
            break;
        }
		else
		{
            break;
        }
    }

    std::cout << "Executable: " << m_executable << std::endl;
    std::cout << "Parameters: ";
    for (std::string pair : m_params) {
        // const std::string& key = pair.first;
        const std::string& value = pair;
        
        std::cout << "Value: " << value << std::endl;
    }
    std::cout << std::endl;

    return ;
}

CGI::CGI(std::string path, std::string body)
{
	m_originalPath = path;
	m_executable = path;
	std::string paramString = body;
    
	size_t startPos = 0;
    while (startPos < paramString.length()) {
        size_t equalsPos = paramString.find('=', startPos);
        size_t ampersandPos = paramString.find('&', startPos);

        if (equalsPos != std::string::npos && ampersandPos != std::string::npos)
		{
            std::string paramName = paramString.substr(startPos, equalsPos - startPos);
            std::string paramValue = paramString.substr(equalsPos + 1, ampersandPos - equalsPos - 1);
            m_params.push_back(paramValue);
            startPos = ampersandPos + 1;
        }
		else if (equalsPos != std::string::npos)
		{
            std::string paramName = paramString.substr(startPos, equalsPos - startPos);
            std::string paramValue = paramString.substr(equalsPos + 1);
            m_params.push_back(paramValue);
            break;
        }
		else
		{
            break;
        }
    }

    std::cout << "Executable: " << m_executable << std::endl;
    std::cout << "Parameters: ";
    for (std::string pair : m_params) {
        // const std::string& key = pair.first;
        const std::string& value = pair;
        
        std::cout << "Value: " << value << std::endl;
    }
    std::cout << std::endl;

    return ;
}


void CGI::execute(void)
{
    int	pipe_fd[2];
    pid_t pid{};

    if (pipe(pipe_fd) == -1)
        throw;
    pid = fork();
    if (pid == -1)
        throw;
    if (pid == 0)
    {
        if (close(pipe_fd[0]) == -1)
            throw;
        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
            throw;
        if (close(pipe_fd[1]) == -1)
            throw;
        std::vector<const char*> args;
        args.push_back(PATH_TO_EXEC_PY);
        args.push_back(m_executable.c_str());
        for (const std::string& param : m_params)
            args.push_back(param.c_str());
        args.push_back(nullptr);
        if (execve(PATH_TO_EXEC_PY, const_cast<char* const*>(args.data()), nullptr))
        {
            throw HttpStatusCodeException(400);
        }
    }
    else
    {
        if (close(pipe_fd[1]) == -1)
            throw;
        if (read(pipe_fd[0], m_readBuf, 256) == -1) //Fix BUFSIZE
            throw;
        if (close(pipe_fd[0]) == -1)
            throw;
    }
    return ;
}