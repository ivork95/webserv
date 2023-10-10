
#include "UtilsConfig.hpp"

/**
 * TODO valid cgi extension? py, php, c?
 * ! eval sheet tries .bla extension
*/
bool	isValidCgiExtension(const std::string &str) {
	if (str.size() > 1)
		return true;
	if (str == ".php" || str == ".py" || str == ".c")
		return true;
	return false;
}
