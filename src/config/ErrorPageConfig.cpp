
#include "ErrorPageConfig.hpp"

/**
 * CONSTRUCTORS / DESTRUCTORS
*/
ErrorPageConfig::ErrorPageConfig(void) : _errorCodes{}, _uriPath{} {
	// std::cout << "ErrorPageConfig default constructor called\n";
}

ErrorPageConfig::ErrorPageConfig(const std::vector<int> &errorCode, const std::string &uriPath) : \
	_errorCodes(errorCode), _uriPath(uriPath) {
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

const std::string	&ErrorPageConfig::getUriPath(void) const {
	return (_uriPath);
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
	out << "\turiPath: " << errorPage.getUriPath() << std::endl;
	return out;
}

/**
 * MEMBER FUNCTIONS
*/
