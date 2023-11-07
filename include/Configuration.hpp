#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <iostream>
#include <vector>

#include "ServerConfig.hpp"
#include "UtilsConfig.hpp"
#include "Logger.hpp"

class Configuration
{
public:
    // default constructor
    Configuration(void);

    std::vector<ServerConfig> serversConfig;
    std::vector<std::string> serverSections;

	void printConfig(void);
};

int initConfig(const std::string &filePath, Configuration &config);

#endif
