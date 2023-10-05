
#include "LocationConfig.hpp"

/**
 * CONSTRUCTORS / DESTRUCTORS
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
	_requestURI{}, _rootPath{}, _clientMaxBodySize{}, \
	_autoIndex(false), _indexFile{}, _cgiHandler{}, _httpMethods{} {
	// std::cout << "LocationConfig default constructor called\n";
}

LocationConfig::LocationConfig(const std::string &requestURI) : \
	_requestURI(requestURI), _hasRequestURI(true), _rootPath{}, _clientMaxBodySize{}, \
	_autoIndex(false), _indexFile{}, _cgiHandler{}, _httpMethods{} {
	// std::cout << "LocationConfig parametric constructor called\n";
}

LocationConfig::~LocationConfig(void) {
	// std::cout << "LocationConfig destructor called\n";
}

/**
 * GETTERS / SETTERS
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
	if (_hasRequestURI)
		throw AlreadySetException("request URI");
	_requestURI = requestURI;
	_hasRequestURI = true;
}

void LocationConfig::setRootPath(const std::string &rootPath) {
	if (_hasRootPath)
		throw AlreadySetException("root path");
	_rootPath = rootPath;
	_hasRootPath = true;
}

void LocationConfig::setClientMaxBodySize(const std::string &clientMaxBodySize) {
	if (_hasClientMaxBodySize)
		throw AlreadySetException("client max body size");
	_clientMaxBodySize = clientMaxBodySize;
	_hasClientMaxBodySize = true;
}

void LocationConfig::setAutoIndex(const bool &autoIndex) {
	if (_hasAutoIndex)
		throw AlreadySetException("auto index");
	_autoIndex = autoIndex;
	_hasAutoIndex = true;
}

void LocationConfig::setIndexFile(const std::vector<std::string> &indexFile) {
	if (_hasIndexFile)
		throw AlreadySetException("index file");
	_indexFile = indexFile;
	_hasIndexFile = true;
}

void LocationConfig::setCgiHandler(const std::string &cgiExtension, const std::string &cgiPath) {
	// if (_hasCgiHandler)
	// 	throw DirectiveAlreadySetException("CGI handler"); // ! there can be multiple cgi handlers
	_cgiHandler[cgiExtension] = cgiPath;
	_hasCgiHandler = true; // ! needed ?
}

void LocationConfig::setHttpMethods(const std::vector<std::string> &httpMethods) {
	if (_hasHttpMethods)
		throw AlreadySetException("HTTP methods");
	_httpMethods = httpMethods;
	_hasHttpMethods = true; // ! needed ?
}

bool LocationConfig::hasRequestURI(void) const {
	return (_hasRequestURI);
}

bool LocationConfig::hasRootPath(void) const {
	return (_hasRootPath);
}

bool LocationConfig::hasClientMaxBodySize(void) const {
	return (_hasClientMaxBodySize);
}

bool LocationConfig::hasAutoIndex(void) const {
	return (_hasAutoIndex);
}

bool LocationConfig::hasIndexFile(void) const {
	return (_hasIndexFile);
}

bool LocationConfig::hasCgiHandler(void) const {
	return (_hasCgiHandler);
}

bool LocationConfig::hasHttpMethods(void) const {
	return (_hasHttpMethods);
}

/**
 * OPERATOR OVERLOADS
*/
std::ostream	&operator << (std::ostream &out, const LocationConfig &route) {
	out << "\nLocationConfig:\n";
	out << "\trequestUri: " << route.getRequestURI() << std::endl;
	out << "\trootPath: " << route.getRootPath() << std::endl;
	out << "\tclientMaxBodySize: " << route.getClientMaxBodySize() << std::endl;
	out << "\tautoIndex: " << route.getAutoIndex() << std::endl;
	out << "\tindexFile(s): [";
	for (size_t i = 0; i < route.getIndexFile().size(); ++i) {
		out << route.getIndexFile()[i] << ",";
	}
	out << "]" << std::endl;
	out << "\tcgiHandler(s): [";
	for (std::map<std::string, std::string>::const_iterator it = route.getCgiHandler().begin(); it != route.getCgiHandler().end(); ++it) {
		out << "(" << it->first << ", " << it->second << "), ";
	}
	out << "]" << std::endl;
	out << "\thttpMethod(s): [";
	for (size_t i = 0; i < route.getHttpMethods().size(); ++i) {
		out << route.getHttpMethods()[i] << ",";
	}
	out << "]" << std::endl;
	return out;
}

/**
 * MEMBER FUNCTIONS
 */
void	LocationConfig::checkMissingDirective(void) {
	if (!hasRequestURI()) {
		// default value: -
		// std::cout << "No request URI\n"; // ? debug
	}
	if (!hasRootPath()) {
		// std::cout << "No root path (setting to default)\n"; // ? debug
		setRootPath("html");
	}
	if (!hasClientMaxBodySize()) {
		// std::cout << "No client max body size (setting to default)\n"; // ? debug
		setClientMaxBodySize("1000000");
	}
	if (!hasAutoIndex()) {
		// std::cout << "No auto index (setting to default)\n"; // ? debug
		setAutoIndex(false);
	}
	if (!hasIndexFile()) {
		// std::cout << "No index file (setting to default)\n"; // ? debug
		setIndexFile({"index.html"});
	}
	if (!hasCgiHandler()) {
		// default value: -
		// std::cout << "No CGI handler\n"; // ? debug
	}
	if (!hasHttpMethods()) {
		// default value: -
		// std::cout << "No HTTP methods\n"; // ? debug
	}
}
