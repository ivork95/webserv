
#include "ErrorPageConfig.hpp"

/**
 * CONSTRUCTORS / DESTRUCTORS
*/
ErrorPageConfig::ErrorPageConfig(void) : _errorCode(), _filePath("N/A") {
	// std::cout << "ErrorPageConfig default constructor called\n";
}

ErrorPageConfig::ErrorPageConfig(const std::vector<std::string> &errorCode, const std::string &filePath) : \
	_errorCode{errorCode}, _filePath(filePath) {
	// std::cout << "ErrorPageConfig parametric constructor called\n";
}

ErrorPageConfig::~ErrorPageConfig(void) {
	// std::cout << "ErrorPageConfig destructor called\n";
}

/**
 * Getters / setters
*/
const std::vector<std::string>	&ErrorPageConfig::getErrorCode(void) const {
	return (_errorCode);
}

const std::string	&ErrorPageConfig::getFilePath(void) const {
	return (_filePath);
}

/**
 * OPERATOR OVERLOADS
*/
std::ostream	&operator << (std::ostream &out, const ErrorPageConfig &errorPage) {
	out << "\nErrorPageConfig:\n";
	out << "\terrorCode: [";
	for (size_t i = 0; i < errorPage.getErrorCode().size(); ++i) {
		out << errorPage.getErrorCode()[i] << ",";
	}
	out << "]" << std::endl;
	out << "\tfilePath: " << errorPage.getFilePath() << std::endl;
	return out;
}

/**
 * MEMBER FUNCTIONS
*/
