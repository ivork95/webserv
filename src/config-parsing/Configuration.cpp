
#include "Configuration.hpp"

/**
 * Constructors / destructors
*/
Route::Route(void) : routePath(""), fileOrDirectory("") {
	// std::cout << "Route default constructor called\n";
}

Route::Route(std::string routePath) : routePath(routePath), fileOrDirectory("") {
	// std::cout << "Route parametric constructor called\n";
}

Route::~Route(void) {
	// std::cout << "Route destructor called\n";
}

Server::Server(void) : _index(0), port(""), serverName(""), clientBodySizeLimit(-1), rawData("") {
	// std::cout << "Server default constructor called\n";
}

Server::Server(unsigned int index, std::string rawData) : \
	_index(index), port(""), serverName(""), clientBodySizeLimit(-1), rawData(rawData) {
	// std::cout << "Server parametric constructor called\n";
}

Server::~Server(void) {
	// std::cout << "Server destructor called\n";
}

Configuration::Configuration(void) : servers(), serverSections() {
	// std::cout << "Configuration default constructor called\n";
}

Configuration::~Configuration(void) {
	// std::cout << "Configuration destructor called\n";
}

/**
 * Member functions
*/
void	Server::printData(void) {
	std::cout << "Server " << _index << ":\n";
	std::cout << "\tport: " << port << std::endl;
	std::cout << "\tserverName: " << serverName << std::endl;
	std::cout << "\terror_pages:\n";
	for (size_t i = 0; i < errorPages.size(); ++i) {
		std::cout << "\t\t" << errorPages[i] << std::endl;
	}
	std::cout << "\troutes:\n";
	for (size_t i = 0; i < routes.size(); ++i) {
		std::cout << "\t\t" << routes[i].routePath << " | " << routes[i].fileOrDirectory << std::endl;
	}
	std::cout << "\tclientBodySizeLimit: " << clientBodySizeLimit << std::endl;
	std::cout << "\trawData: " << rawData << std::endl;
	std::cout << "\ttokens:\n";
	for (size_t i = 0; i < tokens.size(); ++i) {
		std::cout << "\t\t" << tokens[i]._getWord() << " | " << tokens[i]._getType() << std::endl;
	}
	std::cout << std::endl;
}