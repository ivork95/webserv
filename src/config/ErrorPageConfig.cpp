
#include "ErrorPageConfig.hpp"

/**
 * CONSTRUCTORS / DESTRUCTORS
*/
ErrorPageConfig::ErrorPageConfig(void) : _errorCodes{}, _filePath{} {
	// std::cout << "ErrorPageConfig default constructor called\n";
}

ErrorPageConfig::ErrorPageConfig(const std::vector<int> &errorCode, const std::string &filePath) : \
	_errorCodes(errorCode), _filePath(filePath) {
	// std::cout << "ErrorPageConfig parametric constructor called\n";
}

ErrorPageConfig::~ErrorPageConfig(void) {
	// std::cout << "ErrorPageConfig destructor called\n";
}

/**
 * GETTERS / SETTERS
*/
const std::vector<int>	&ErrorPageConfig::getErrorCodes(void) const {
	return (_errorCodes);
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
	for (size_t i = 0; i < errorPage.getErrorCodes().size(); ++i) {
		out << errorPage.getErrorCodes()[i] << ",";
	}
	out << "]" << std::endl;
	out << "\tfilePath: " << errorPage.getFilePath() << std::endl;
	return out;
}

/**
 * MEMBER FUNCTIONS
*/
