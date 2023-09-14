
#include "UtilsConfig.hpp"

/**
 * TODO valid error codes? reserved values?
*/
bool	isValidErrorCode(const std::string &str) {
	if (str.size() != 3)
		return false;
	if (!isNumber(str))
		return false;
	return true;
}
