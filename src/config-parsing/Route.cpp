
#include "Route.hpp"

Route::Route(void) : routePath(""), rootDir(""), clientMaxBodySize(""), autoIndex(false), indexFile(), cgiExtension(""), httpMethods() {
	// std::cout << "Route default constructor called\n";
}

Route::Route(const std::string routePath) : routePath(routePath), rootDir(""), clientMaxBodySize(""), autoIndex(false), indexFile(), cgiExtension(""), httpMethods() {
	// std::cout << "Route parametric constructor called\n";
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
	for (size_t i = 0; i < indexFile.size(); ++i) {
		std::cout << "\t\t\t" << indexFile[i] << std::endl;
	}
	std::cout << "\t\tcgiExtension: " << cgiExtension << std::endl;
	std::cout << "\t\thttpMethods:\n";
	for (size_t i = 0; i < httpMethods.size(); ++i) {
		std::cout << "\t\t" << httpMethods[i] << std::endl;
	}
	std::cout << std::endl;
}
