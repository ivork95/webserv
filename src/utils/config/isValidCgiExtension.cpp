
#include "UtilsConfig.hpp"

/**
 * TODO valid cgi extension? py, php, c?
*/
bool	isValidCgiExtension(const std::string &str) {
	if (str == ".php" || str == ".py" || str == ".c")
		return true;
	return false;
}
