
#include "ErrorPageConfig.hpp"

/**
 * CONSTRUCTORS / DESTRUCTORS
*/
ErrorPageConfig::ErrorPageConfig(void) : _errorCode{}, _uriPath{} {
	// std::cout << "ErrorPageConfig default constructor called\n";
}

ErrorPageConfig::ErrorPageConfig(const std::vector<std::string> &errorCode, const std::string &uriPath) : \
	_errorCode(errorCode), _uriPath(uriPath) {
	// std::cout << "ErrorPageConfig parametric constructor called\n";
}

ErrorPageConfig::~ErrorPageConfig(void) {
	// std::cout << "ErrorPageConfig destructor called\n";
}

/**
 * GETTERS / SETTERS
*/
const std::vector<std::string>	&ErrorPageConfig::getErrorCode(void) const {
	return (_errorCode);
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
	for (size_t i = 0; i < errorPage.getErrorCode().size(); ++i) {
		out << errorPage.getErrorCode()[i] << ",";
	}
	out << "]" << std::endl;
	out << "\turiPath: " << errorPage.getUriPath() << std::endl;
	return out;
}

/**
 * MEMBER FUNCTIONS
*/
