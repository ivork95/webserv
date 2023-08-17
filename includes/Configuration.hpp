#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <iostream>
#include <vector>

#include "Token.hpp"

class Route {
	public:
		Route();
		Route(std::string routePath);
		~Route(void);

		std::string routePath;
		std::string fileOrDirectory;
};

class Server {
	public:
		Server(void);
		Server(unsigned int index, std::string rawData);
		~Server(void);

		unsigned int				_index;
		std::string					port;
		std::string					serverName;
		std::vector<std::string>	errorPages{};
		std::vector<Route>			routes{};
		int							clientBodySizeLimit;
		std::string					rawData;
		std::vector<Token>			tokens{};

		void						printData(void);
};

class Configuration {
	public:
		Configuration(void);
		~Configuration(void);

		std::vector<Server>			servers;
		std::vector<std::string>	serverSections;
};

#endif
