
#include "Route.hpp"

/**
 * Constructors and destructor
 * TODO set default values ?
 * 	requestURI: /;
 * 	root: html;
 * 	client_max_body_size: 1M;
 * 	autoindex: off;
 * 	index: index.html;
 *  cgi_handler: -; => ?
 *  limit_except: -;
 */
Route::Route(void) : \
	_requestURI("N/A"), _rootPath("N/A"), _clientMaxBodySize("N/A"), \
	_autoIndex(false), _indexFile{"N/A"}, _cgiHandler{}, _httpMethods{"N/A"} {
	// std::cout << "Route default constructor called\n";
}

Route::Route(const std::string requestURI) : \
	_requestURI(requestURI), _rootPath("N/A"), _clientMaxBodySize("N/A"), \
	_autoIndex(false), _indexFile{"N/A"}, _cgiHandler{}, _httpMethods{"N/A"} {
	// std::cout << "Route parametric constructor called\n";
	// printData(); // ? testing
}

Route::~Route(void) {
	// std::cout << "Route destructor called\n";
}

/**
 * Getters / setters
*/
std::string Route::getRequestURI(void) const {
	return this->_requestURI;
}

std::string Route::getRootPath(void) const {
	return this->_rootPath;
}

std::string Route::getClientMaxBodySize(void) const {
	return this->_clientMaxBodySize;
}

bool Route::getAutoIndex(void) const {
	return this->_autoIndex;
}

std::vector<std::string> Route::getIndexFile(void) const {
	return this->_indexFile;
}

std::map<std::string, std::string> Route::getCgiHandler(void) const {
	return this->_cgiHandler;
}

std::vector<std::string> Route::getHttpMethods(void) const {
	return this->_httpMethods;
}

/**
 * Operator overloads
*/
std::ostream	&operator << (std::ostream &out, const Route &route) {
	out << "Route:\n";
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
 * Member functions
 */
// TODO make printData virtual in all classes (see CPP modules)
// void	Route::printData(void) {
// 	std::cout << "\tRoute:\n";
// 	std::cout << "\t\trequestURI: " << requestURI << std::endl;
// 	std::cout << "\t\trootPath: " << rootPath << std::endl;
// 	std::cout << "\t\tclientMaxBodySize: " << clientMaxBodySize << std::endl;
// 	std::cout << "\t\tautoIndex: " << autoIndex << std::endl;
// 	std::cout << "\t\tindexFile: [";
// 	for (size_t i = 0; i < indexFile.size(); ++i) {
// 		std::cout << indexFile[i] << ",";
// 	}
// 	std::cout << "]" << std::endl;
// 	std::cout << "\t\tcgiHandler: ";
// 	for (std::map<std::string, std::string>::iterator it = cgiHandler.begin(); it != cgiHandler.end(); ++it) {
// 		std::cout << "[" << it->first << ", " << it->second << "], ";
// 	}
// 	std::cout << "\n\t\thttpMethods: [";
// 	for (size_t i = 0; i < httpMethods.size(); ++i) {
// 		std::cout << httpMethods[i] << ",";
// 	}
// 	std::cout << "]" << std::endl;
// }
