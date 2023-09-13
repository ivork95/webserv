#ifndef CGI_HPP
#define CGI_HPP

# include <string>
# include <vector>
# include <iostream>

class CGI
{
public:

	CGI (std::string path);
	// ~CGI (void);
	std::string m_originalPath{};
	std::string m_executable{};
	std::vector<std::string> m_params{};

};

#endif


CGI::CGI(std::string path)
{
	m_originalPath = path;
	size_t posQM = path.find("?");
	m_executable = path.substr(9, posQM - 9);
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

