
#include "ServerConfig.hpp"

/**
 * Constructors / destructors
*/
/**
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
ServerConfig::ServerConfig(void) : \
	_index(0), _portNb("N/A"), _serverName("N/A"), _clientMaxBodySize("N/A"), \
	_errorPages(), _routesConfig(), _rawData("N/A") {
	// std::cout << "ServerConfig default constructor called\n";
}

ServerConfig::ServerConfig(unsigned int index, std::string rawData) : \
	_index(index), _portNb("N/A"), _serverName("N/A"), _clientMaxBodySize("N/A"), \
	_errorPages(), _routesConfig(), _rawData(rawData) {
	// std::cout << "ServerConfig parametric constructor called\n";
}

ServerConfig::~ServerConfig(void) {
	// std::cout << "ServerConfig destructor called\n";
}

/**
 * Getters / setters
*/
const unsigned int &ServerConfig::getIndex(void) const {
	return (_index);
}

const std::string &ServerConfig::getPortNb(void) const {
	return (_portNb);
}

const std::string &ServerConfig::getServerName(void) const {
	return (_serverName);
}

const std::string &ServerConfig::getClientMaxBodySize(void) const {
	return (_clientMaxBodySize);
}

const std::vector<ErrorPageConfig> &ServerConfig::getErrorPages(void) const {
	return (_errorPages);
}

const std::vector<LocationConfig> &ServerConfig::getRoutesConfig(void) const {
	return (_routesConfig);
}

const std::string &ServerConfig::getRawData(void) const {
	return (_rawData);
}

const std::vector<Token> &ServerConfig::getTokens(void) const {
	return (_tokens);
}

void ServerConfig::setIndex(const unsigned int index) {
	_index = index;
}

void ServerConfig::setPortNb(const std::string &portNb) {
	_portNb = portNb;
}

void ServerConfig::setServerName(const std::string &serverName) {
	_serverName = serverName;
}

void ServerConfig::setClientMaxBodySize(const std::string &clientMaxBodySize) {
	_clientMaxBodySize = clientMaxBodySize;
}

void ServerConfig::setErrorPages(const ErrorPageConfig &errorPages) {
	_errorPages.push_back(errorPages);
}

void ServerConfig::setRoutesConfig(const LocationConfig &routeConfig) {
	_routesConfig.push_back(routeConfig);
}

void ServerConfig::setRawData(const std::string &rawData) {
	_rawData = rawData;
}

void ServerConfig::setTokens(const std::vector<Token> &tokens) {
	_tokens = tokens;
}

/**
 * Operator overloads
*/
std::ostream &operator << (std::ostream &out, const ServerConfig &server) {
	out << "=============================================================\n";
	out << "ServerConfig " << server.getIndex() << ":\n";
	out << "\tportNb: " << server.getPortNb() << std::endl;
	out << "\tserverName: " << server.getServerName() << std::endl;
	out << "\tclientBodySizeLimit: " << server.getClientMaxBodySize() << std::endl;
	if (server.getErrorPages().size() == 0) {
		out << "\terrorPages: empty\n";
	}
	for (size_t i = 0; i < server.getErrorPages().size(); ++i) {
		// server.getErrorPages()[i].printData();
		out << server.getErrorPages()[i];
	}
	if (server.getRoutesConfig().size() == 0) {
		out << "\tlocationConfig: empty\n";
	}
	for (size_t i = 0; i < server.getRoutesConfig().size(); ++i) {
		// server.getRoutesConfig()[i].printData();
		out << server.getRoutesConfig()[i];
	}
	// out << "\trawData: " << server.getRawData() << std::endl;
	// out << "\ttokens:\n";
	// for (size_t i = 0; i < server.getTokens().size(); ++i) {
	// 	out << "\t\t" << server.getTokens()[i]._getWord() << " | " << server.getTokens()[i]._getType() << std::endl;
	// }
	out << "=============================================================\n\n";
	return out;
}

/**
 * Member functions
*/
