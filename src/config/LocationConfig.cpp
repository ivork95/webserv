
#include "LocationConfig.hpp"

/**
 * CONSTRUCTORS / DESTRUCTORS
 */
LocationConfig::LocationConfig(void) : \
	_requestURI{}, _rootPath{}, _clientMaxBodySize{}, _autoIndex(false), \
	_indexFile{}, _cgiScript{}, _cgiInterpreter{}, _absCgiScript{}, _httpMethods{} {
	// std::cout << "LocationConfig default constructor called\n";
}

LocationConfig::LocationConfig(const std::string &requestURI) : \
	_requestURI(requestURI), _hasRequestURI(true), _rootPath{}, _clientMaxBodySize{}, \
	_autoIndex(false), _indexFile{}, _cgiScript{}, _cgiInterpreter{}, _absCgiScript{}, _httpMethods{} {
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

const int &LocationConfig::getClientMaxBodySize(void) const {
	return (_clientMaxBodySize);
}

const bool &LocationConfig::getAutoIndex(void) const {
	return (_autoIndex);
}

const std::vector<std::string> &LocationConfig::getIndexFile(void) const {
	return (_indexFile);
}

const std::string &LocationConfig::getCgiScript(void) const {
	return (_cgiScript);
}

const std::string &LocationConfig::getCgiInterpreter(void) const {
	return (_cgiInterpreter);
}

const std::string &LocationConfig::getAbsCgiScript(void) const {
	return (_absCgiScript);
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

void LocationConfig::setClientMaxBodySize(const int &clientMaxBodySize) {
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

void LocationConfig::setCgiScript(const std::string &cgiScript) {
	if (_hasCgiScript)
		throw AlreadySetException("CGI script");
	_cgiScript = cgiScript;
	_hasCgiScript = true;
}

void LocationConfig::setCgiInterpreter(const std::string &cgiInterpreter) {
	if (_hasCgiInterpreter)
		throw AlreadySetException("CGI interpreter");
	_cgiInterpreter = cgiInterpreter;
	_hasCgiInterpreter = true;
}

void LocationConfig::setAbsCgiScript(const std::string &cgiScriptPath) {
	if (_hasAbsCgiScript)
		throw AlreadySetException("CGI script absolute path");
	_absCgiScript = cgiScriptPath;
	_hasAbsCgiScript = true;
}

void LocationConfig::setHttpMethods(const std::vector<std::string> &httpMethods) {
	if (_hasHttpMethods)
		throw AlreadySetException("HTTP methods");
	_httpMethods = httpMethods;
	_hasHttpMethods = true;
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

bool LocationConfig::hasCgiScript(void) const {
	return (_hasCgiScript);
}

bool LocationConfig::hasCgiInterpreter(void) const {
	return (_hasCgiInterpreter);
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
	if (route.hasCgiInterpreter() && route.hasCgiScript()) {
		out << "\tcgiScript: " << route.getCgiScript() << " => " << route.getAbsCgiScript() << std::endl;
		out << "\tabsCgiScript: " << route.getAbsCgiScript() << std::endl;
		// out << "\tcgiInterpreter: " << route.getCgiInterpreter() << std::endl;
	}
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
	if (!hasRootPath()) {
		// std::cout << "No root path (setting to default)\n"; // ? debug
		setRootPath("www/");
	}
	if (!hasClientMaxBodySize()) {
		// std::cout << "No client max body size (setting to default)\n"; // ? debug
		setClientMaxBodySize(1000000);
	}
	if (!hasAutoIndex()) {
		// std::cout << "No auto index (setting to default)\n"; // ? debug
		setAutoIndex(false);
	}
	if (!hasIndexFile()) {
		// std::cout << "No index file (setting to default)\n"; // ? debug
		setIndexFile({"index.html"});
	}
}
