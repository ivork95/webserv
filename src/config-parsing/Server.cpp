
#include "Server.hpp"

/**
 * Constructors / destructors
 * TODO set default values ?
 * 	listen: *:80 | *:8000;
 * 	server_name: "";
 * 	client_max_body_size: 1M;
 * 	error_page: -;
 * 	location: -;
 * 		root: html;
 * 		index: index.html;
 * 		client_max_body_size: 1M;
 *      limit_except: -;
*/
Server::Server(void) : \
	_index(0), portNb("N/A"), serverName("N/A"), clientMaxBodySize("N/A"), \
	errorPages(), routesConfig(), rawData("N/A") {
	// std::cout << "Server default constructor called\n";
}

Server::Server(unsigned int index, std::string rawData) : \
	_index(index), portNb("N/A"), serverName("N/A"), clientMaxBodySize("N/A"), \
	errorPages(), routesConfig(), rawData(rawData) {
	// std::cout << "Server parametric constructor called\n";
	// printData(); // ? testing
}

Server::~Server(void) {
	// std::cout << "Server destructor called\n";
}

/**
 * Member functions
*/
void	Server::printData(void) {
	std::cout << "Server " << _index << ":\n";
	std::cout << "\tportNb: " << portNb << std::endl;
	std::cout << "\tserverName: " << serverName << std::endl;
	std::cout << "\tclientBodySizeLimit: " << clientMaxBodySize << std::endl;
	if (errorPages.size() == 0) {
		std::cout << "\terrorPages: empty\n";
	}
	for (size_t i = 0; i < errorPages.size(); ++i) {
		errorPages[i].printData();
	}
	if (routesConfig.size() == 0) {
		std::cout << "\troutesConfig: empty\n";
	}
	for (size_t i = 0; i < routesConfig.size(); ++i) {
		routesConfig[i].printData();
	}
	// std::cout << "\trawData: " << rawData << std::endl;
	// std::cout << "\ttokens:\n";
	// for (size_t i = 0; i < tokens.size(); ++i) {
	// 	std::cout << "\t\t" << tokens[i]._getWord() << " | " << tokens[i]._getType() << std::endl;
	// }
	std::cout << std::endl;
}
