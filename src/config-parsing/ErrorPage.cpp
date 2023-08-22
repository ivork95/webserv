
#include "ErrorPage.hpp"

/**
 * Constructors / destructors
*/
ErrorPage::ErrorPage(void) : errorCode(), filePath("") {
	// std::cout << "ErrorPage default constructor called\n";
}

ErrorPage::ErrorPage(const std::string errorCode, const std::string filePath) : errorCode{errorCode}, filePath(filePath) {
	// std::cout << "ErrorPage parametric constructor called\n";
}

ErrorPage::~ErrorPage(void) {
	// std::cout << "ErrorPage destructor called\n";
}

/**
 * Member functions
*/
void	ErrorPage::printData(void) {
	std::cout << "\tErrorPage:\n";
	std::cout << "\t\terrorCode: ";
	for (size_t i = 0; i < errorCode.size(); ++i) {
		std::cout << errorCode[i] << std::endl;
	}
	std::cout << "\t\tfilePath: " << filePath << std::endl;
	std::cout << std::endl;
}
