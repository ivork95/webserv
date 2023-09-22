
#include "UtilsConfig.hpp"

bool	isValidHttpMethod(const std::string &str) {
	if (str == "GET" || str == "POST" || str == "DELETE")
		return true;
	return false;
}
