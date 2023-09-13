
#include "UtilsConfig.hpp"

/**
 * TODO valid error codes? reserved values?
*/
bool	isValidErrorCode(const std::string &errorCode) {
	if (errorCode.size() != 3)
		return false;
	if (!isdigit(errorCode[0]) || !isdigit(errorCode[1]) || !isdigit(errorCode[2]))
		return false;
	return true;
}
