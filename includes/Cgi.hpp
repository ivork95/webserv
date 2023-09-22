#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <vector>
#include <iostream>

#define PATH_TO_EXEC_PY "/usr/bin/python3"

class CGI
{
public:
    CGI(std::string path);
    CGI(std::string path, std::string body);
    // ~CGI (void);

    void execute(void);
    std::string m_originalPath{};
    std::string m_executable{};
    char m_readBuf[256]{};
    std::vector<std::string> m_params{};
};

#endif