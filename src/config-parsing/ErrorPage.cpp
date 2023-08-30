
#include "ErrorPage.hpp"

/**
 * Constructors / destructors
*/
ErrorPage::ErrorPage(void) : _errorCode(), _filePath("N/A") {
	// std::cout << "ErrorPage default constructor called\n";
}

ErrorPage::ErrorPage(const std::vector<std::string> &errorCode, const std::string &filePath) : \
	_errorCode{errorCode}, _filePath(filePath) {
	// std::cout << "ErrorPage parametric constructor called\n";
}

ErrorPage::~ErrorPage(void) {
	// std::cout << "ErrorPage destructor called\n";
}

/**
 * Getters / setters
*/
std::vector<std::string>	ErrorPage::getErrorCode(void) const {
	return this->_errorCode;
}

std::string	ErrorPage::getFilePath(void) const {
	return this->_filePath;
}

/**
 * Operator overloads
*/
std::ostream	&operator << (std::ostream &out, const ErrorPage &errorPage) {
	out << "ErrorPage:\n";
	out << "\terrorCode: [";
	for (size_t i = 0; i < errorPage.getErrorCode().size(); ++i) {
		out << errorPage.getErrorCode()[i] << ",";
	}
	out << "]" << std::endl;
	out << "\t\tfilePath: " << errorPage.getFilePath() << std::endl;
	return out;
}

/**
 * Member functions
*/
// void	ErrorPage::printData(void) {
// 	std::cout << "\tErrorPage:\n";
// 	std::cout << "\t\terrorCode: [";
// 	for (size_t i = 0; i < _errorCode.size(); ++i) {
// 		std::cout << _errorCode[i] << ",";
// 	}
// 	std::cout << "]" << std::endl;
// 	std::cout << "\t\tfilePath: " << _filePath << std::endl;
// }
