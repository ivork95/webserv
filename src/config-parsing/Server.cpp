
#include "Server.hpp"

/**
 * Constructors / destructors
*/
Server::Server(void) : _index(0), portNb(""), serverName(""), errorPages(), routesConfig(), clientMaxBodySize(""), rawData("") {
	// std::cout << "Server default constructor called\n";
}

Server::Server(unsigned int index, std::string rawData) : \
	_index(index), portNb(""), serverName(""), errorPages(), routesConfig(), clientMaxBodySize(""), rawData(rawData) {
	// std::cout << "Server parametric constructor called\n";
}

Server::~Server(void) {
	// std::cout << "Server destructor called\n";
}

/**
 * Member functions
*/
void	Server::printData(void) {
	std::cout << "Server " << _index << ":\n";
	std::cout << "\tport: " << portNb << std::endl;
	std::cout << "\tserverName: " << serverName << std::endl;
	for (size_t i = 0; i < errorPages.size(); ++i) {
		errorPages[i].printData();
	}
	for (size_t i = 0; i < routesConfig.size(); ++i) {
		routesConfig[i].printData();
		// std::cout << "\t\t" << routesConfig[i].routePath << std::endl;
	}
	std::cout << "\tclientBodySizeLimit: " << clientMaxBodySize << std::endl;
	// std::cout << "\trawData: " << rawData << std::endl;
	// std::cout << "\ttokens:\n";
	// for (size_t i = 0; i < tokens.size(); ++i) {
	// 	std::cout << "\t\t" << tokens[i]._getWord() << " | " << tokens[i]._getType() << std::endl;
	// }
	std::cout << std::endl;
}
