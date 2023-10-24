
#include "UtilsConfig.hpp"

bool	isValidUri(const std::string &str) {
	if (str.empty() || str.front() != '/')
		return false;
	if (str == "/")
		return true;
	return true;
}
