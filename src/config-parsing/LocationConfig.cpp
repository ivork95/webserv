
#include "LocationConfig.hpp"

/**
 * Constructors and destructor
 */
/**
 * TODO set default values ?
 * 	requestURI: /;
 * 	root: html;
 * 	client_max_body_size: 1M;
 * 	autoindex: off;
 * 	index: index.html;
 *  cgi_handler: -; => ?
 *  limit_except: -;
*/
LocationConfig::LocationConfig(void) : \
	_requestURI("N/A"), _rootPath("N/A"), _clientMaxBodySize("N/A"), \
	_autoIndex(false), _indexFile{"N/A"}, _cgiHandler{}, _httpMethods{"N/A"} {
	// std::cout << "LocationConfig default constructor called\n";
}

LocationConfig::LocationConfig(const std::string &requestURI) : \
	_requestURI(requestURI), _rootPath("N/A"), _clientMaxBodySize("N/A"), \
	_autoIndex(false), _indexFile{"N/A"}, _cgiHandler{}, _httpMethods{"N/A"} {
	// std::cout << "LocationConfig parametric constructor called\n";
}

LocationConfig::~LocationConfig(void) {
	// std::cout << "LocationConfig destructor called\n";
}

/**
 * Getters / setters
*/
const std::string &LocationConfig::getRequestURI(void) const {
	return (_requestURI);
}

const std::string &LocationConfig::getRootPath(void) const {
	return (_rootPath);
}

const std::string &LocationConfig::getClientMaxBodySize(void) const {
	return (_clientMaxBodySize);
}

const bool &LocationConfig::getAutoIndex(void) const {
	return (_autoIndex);
}

const std::vector<std::string> &LocationConfig::getIndexFile(void) const {
	return (_indexFile);
}

const std::map<std::string, std::string> &LocationConfig::getCgiHandler(void) const {
	return (_cgiHandler);
}

const std::vector<std::string> &LocationConfig::getHttpMethods(void) const {
	return (_httpMethods);
}

void LocationConfig::setRequestURI(const std::string &requestURI) {
	_requestURI = requestURI;
}

void LocationConfig::setRootPath(const std::string &rootPath) {
	_rootPath = rootPath;
}

void LocationConfig::setClientMaxBodySize(const std::string &clientMaxBodySize) {
	_clientMaxBodySize = clientMaxBodySize;
}

void LocationConfig::setAutoIndex(const bool &autoIndex) {
	_autoIndex = autoIndex;
}

void LocationConfig::setIndexFile(const std::vector<std::string> &indexFile) {
	_indexFile = indexFile;
}

void LocationConfig::setCgiHandler(const std::string &cgiExtension, const std::string &cgiPath) {
	_cgiHandler[cgiExtension] = cgiPath;
}

void LocationConfig::setHttpMethods(const std::vector<std::string> &httpMethods) {
	_httpMethods = httpMethods;
}

/**
 * OPERATOR OVERLOADS
*/
std::ostream	&operator << (std::ostream &out, const LocationConfig &route) {
	out << "\nLocationConfig:\n";
	out << "\trequestURI: " << route.getRequestURI() << std::endl;
	out << "\trootPath: " << route.getRootPath() << std::endl;
	out << "\tclientMaxBodySize: " << route.getClientMaxBodySize() << std::endl;
	out << "\tautoIndex: " << route.getAutoIndex() << std::endl;
	out << "\tindexFile: [";
	for (size_t i = 0; i < route.getIndexFile().size(); ++i) {
		out << route.getIndexFile()[i] << ",";
	}
	out << "]" << std::endl;
	out << "\tcgiHandler: ";
	for (std::map<std::string, std::string>::const_iterator it = route.getCgiHandler().begin(); it != route.getCgiHandler().end(); ++it) {
		out << "[" << it->first << ", " << it->second << "], ";
	}
	out << "\n\thttpMethods: [";
	for (size_t i = 0; i < route.getHttpMethods().size(); ++i) {
		out << route.getHttpMethods()[i] << ",";
	}
	out << "]" << std::endl;
	return out;
}

/**
 * MEMBER FUNCTIONS
 */
