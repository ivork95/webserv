
#include "UtilsConfig.hpp"

bool	isValidErrorCode(const int &errorCode) {
	if (errorCode < 400 || errorCode > 599)
		return false;
	return true;
}
