#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <iostream>
#include <vector>

class Route {
	public:
		// Route(void) = delete;
		Route();
		~Route(void);

		std::string routePath;
		std::string fileOrDirectory;
};

class Server {
	public:
		// Server(void) = delete;
		Server();
		~Server(void);

		std::vector<std::string>	serverName;
		std::vector<std::string>	errorPages;
		std::vector<Route>			routes;
		int							port;
		int							clientBodySizeLimit;
};

class Configuration {
	public:
		// Configuration(void) = delete;
		Configuration();
		~Configuration(void);

		std::vector<Server> m_servers;
};

#endif
