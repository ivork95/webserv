
#include "Route.hpp"

Route::Route(void) : \
	routePath("N/A"), rootDir("N/A"), clientMaxBodySize("N/A"), \
	autoIndex(false), indexFile{"N/A"}, cgiHandler{{"N/A","N/A"}}, httpMethods{"N/A"} {
	// std::cout << "Route default constructor called\n";
}

Route::Route(const std::string routePath) : \
	routePath(routePath), rootDir("N/A"), clientMaxBodySize("N/A"), \
	autoIndex(false), indexFile{"N/A"}, cgiHandler{{"N/A","N/A"}}, httpMethods{"N/A"} {
	// std::cout << "Route parametric constructor called\n";
	// printData(); // ? testing
}

Route::~Route(void) {
	// std::cout << "Route destructor called\n";
}

/**
 * Member functions
 */
// TODO make printData virtual in all classes (see CPP modules)
void	Route::printData(void) {
	std::cout << "\tRoute:\n";
	std::cout << "\t\troutePath: " << routePath << std::endl;
	std::cout << "\t\trootDir: " << rootDir << std::endl;
	std::cout << "\t\tclientMaxBodySize: " << clientMaxBodySize << std::endl;
	std::cout << "\t\tautoIndex: " << autoIndex << std::endl;
	std::cout << "\t\tindexFile: ";
	for (size_t i = 0; i < indexFile.size(); ++i) {
		std::cout << indexFile[i];
	}
	std::cout << "\n\t\tcgiHandler: ";
	for (std::map<std::string, std::string>::iterator it = cgiHandler.begin(); it != cgiHandler.end(); ++it) {
		std::cout << "[" << it->first << ", " << it->second << "]" << std::endl;
	}
	std::cout << "\t\thttpMethods: ";
	for (size_t i = 0; i < httpMethods.size(); ++i) {
		std::cout << httpMethods[i];
	}
	std::cout << std::endl;
}
