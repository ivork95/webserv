
#include "ServerConfig.hpp"

/**
 * CONSTRUCTORS / DESTRUCTORS
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
	_index{}, _portNb{}, _serverName{}, _clientMaxBodySize{}, \
	_errorPagesConfig{}, _locationsConfig{}, _rawData{} {
	// std::cout << "ServerConfig default constructor called\n";
}

ServerConfig::ServerConfig(unsigned int index, std::string rawData) : \
	_index(index), _portNb{}, _serverName{}, _clientMaxBodySize{}, \
	_errorPagesConfig{}, _locationsConfig{}, _rawData(rawData) {
	// std::cout << "ServerConfig parametric constructor called\n";
}

ServerConfig::~ServerConfig(void) {
	// std::cout << "ServerConfig destructor called\n";
}

/**
 * GETTERS / SETTERS
*/
const unsigned int &ServerConfig::getIndex(void) const {
	return (_index);
}

const std::string &ServerConfig::getPortNb(void) const {
	return (_portNb);
}

const std::vector<std::string> &ServerConfig::getServerName(void) const {
	return (_serverName);
}

const int &ServerConfig::getClientMaxBodySize(void) const {
	return (_clientMaxBodySize);
}

const std::vector<ErrorPageConfig> &ServerConfig::getErrorPagesConfig(void) const {
	return (_errorPagesConfig);
}

const std::vector<LocationConfig> &ServerConfig::getLocationsConfig(void) const {
	return (_locationsConfig);
}

const std::string &ServerConfig::getRawData(void) const {
	return (_rawData);
}

const std::vector<Token> &ServerConfig::getTokens(void) const {
	return (_tokens);
}

void ServerConfig::setIndex(const unsigned int &index) {
	_index = index;
}

void ServerConfig::setPortNb(const std::string &portNb) {
	if (_hasPortNb) 
		throw AlreadySetException("port number");
	_portNb = portNb;
	_hasPortNb = true;
}

void ServerConfig::setServerName(const std::vector<std::string> &serverName) {
	if (_hasServerName)
		throw AlreadySetException("server name");
	_serverName = serverName;
	_hasServerName = true;
}

void ServerConfig::setClientMaxBodySize(const int &clientMaxBodySize) {
	if (_hasClientMaxBodySize)
		throw AlreadySetException("client max body size");
	_clientMaxBodySize = clientMaxBodySize;
	_hasClientMaxBodySize = true;
}

/**
 * TODO what if multiple directive with same error codes ?
 * TODO what if multiple directive with same error page ?
 * TODO what if multiple directive with same error page and error codes?
*/
void ServerConfig::setErrorPagesConfig(const ErrorPageConfig &errorPages) {
	// if (_hasErrorPagesConfig)
	// 	throw AlreadySetException("error pages"); // ! there can be multiple error pages
	_errorPagesConfig.push_back(errorPages);
	_hasErrorPagesConfig = true;
}

void ServerConfig::setLocationsConfig(const LocationConfig &routeConfig) {
	// if (_hasLocationsConfig)
	// 	throw AlreadySetException("location"); // ! there can be multiple locations
	_locationsConfig.push_back(routeConfig);
	_hasLocationsConfig = true;
}

void ServerConfig::setRawData(const std::string &rawData) {
	_rawData = rawData;
}

void ServerConfig::setTokens(const std::vector<Token> &tokens) {
	_tokens = tokens;
}

bool ServerConfig::hasPortNb(void) const {
	return (_hasPortNb);
}

bool ServerConfig::hasServerName(void) const {
	return (_hasServerName);
}

bool ServerConfig::hasClientMaxBodySize(void) const {
	return (_hasClientMaxBodySize);
}

bool ServerConfig::hasErrorPagesConfig(void) const {
	return (_hasErrorPagesConfig);
}

bool ServerConfig::hasLocationsConfig(void) const {
	return (_hasLocationsConfig);
}

/**
 * OPERATOR OVERLOADS
*/
std::ostream &operator << (std::ostream &out, const ServerConfig &server) {
	out << "=============================================================\n";
	out << "ServerConfig " << server.getIndex() << ":\n";
	out << "\tportNb: " << server.getPortNb() << std::endl;
	out << "\tserverName(s): [";
	for (size_t i = 0; i < server.getServerName().size(); ++i) {
		out << server.getServerName()[i] << ",";
	}
	out << "]" << std::endl;
	out << "\tclientBodySizeLimit: " << server.getClientMaxBodySize() << std::endl;
	if (server.getErrorPagesConfig().size() == 0) {
		out << "\nErrorPageConfig: empty\n";
	}
	for (size_t i = 0; i < server.getErrorPagesConfig().size(); ++i) {
		out << server.getErrorPagesConfig()[i];
	}
	if (server.getLocationsConfig().size() == 0) {
		out << "\nLocationConfig: empty\n";
	}
	for (size_t i = 0; i < server.getLocationsConfig().size(); ++i) {
		out << server.getLocationsConfig()[i];
	}
	// out << "\trawData: " << server.getRawData() << std::endl;
	// out << "\ttokens:\n";
	// for (size_t i = 0; i < server.getTokens().size(); ++i) {
	// 	out << "\t\t" << server.getTokens()[i].getWord() << " | " << server.getTokens()[i].getType() << std::endl;
	// }
	out << "=============================================================\n\n";
	return out;
}

/**
 * MEMBER FUNCTIONS
*/
void	ServerConfig::checkMissingDirective(void) {
	if (!hasPortNb()) {
		// std::cout << "No port number (setting to default)\n"; // ? debug
		setPortNb("80");
	}
	if (!hasServerName()) {
		// std::cout << "No server name (setting to default)\n"; // ? debug
		setServerName({""});
	}
	if (!hasClientMaxBodySize()) {
		// std::cout << "No client max body size (setting to default)\n"; // ? debug
		setClientMaxBodySize(1000000);
	}
	if (!hasErrorPagesConfig()) {
		// default value: -
		// std::cout << "No error pages config\n"; // ? debug
	}
	if (!hasLocationsConfig()) {
		// default value: -
		// std::cout << "No locations config\n"; // ? debug
	}
}
