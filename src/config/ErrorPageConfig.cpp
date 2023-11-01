
#include "ErrorPageConfig.hpp"

/**
 * CONSTRUCTORS / DESTRUCTORS
*/
ErrorPageConfig::ErrorPageConfig(void) : _errorCodes{}, _filePath{} {
	// Logger::getInstance().debug("ErrorPageConfig default constructor called");
}

ErrorPageConfig::ErrorPageConfig(const std::vector<int> &errorCode, const std::string &filePath) : \
	_errorCodes(errorCode), _filePath(filePath) {
	// Logger::getInstance().debug("ErrorPageConfig (" + _filePath + ") parametric constructor called");
}

ErrorPageConfig::~ErrorPageConfig(void) {
	// Logger::getInstance().debug("ErrorPageConfig (" + _filePath + ") destructor called");
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
