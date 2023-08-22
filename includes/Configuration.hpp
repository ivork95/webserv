#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <iostream>
#include <vector>

#include "Server.hpp"

class Configuration {
	public:
		Configuration(void);
		~Configuration(void);

		std::vector<Server>			serversConfig;
		std::vector<std::string>	serverSections;
};

#endif
