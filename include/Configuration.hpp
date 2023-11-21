#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include "ConfigServer.hpp"
#include "Parser.hpp"

class Configuration
{
public:
    // default constructor
    Configuration(void);

    std::vector<ServerConfig> serversConfig;
    std::vector<std::string> serverSections;

	int initConfig(std::ifstream &configFile);

	int readFile(std::ifstream &configFile);
	int createServers(void);
	int tokenizeServers(void);
	int parseTokens(void);

	void printConfig(void);
};


#endif
