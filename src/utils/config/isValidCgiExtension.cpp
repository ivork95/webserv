
#include "UtilsConfig.hpp"

/**
 * TODO valid cgi extension? py, php, c?
*/
bool	isValidCgiExtension(const std::string &cgiExtension) {
	if (cgiExtension == ".php" || cgiExtension == ".py" || cgiExtension == ".c")
		return true;
	return false;
}
