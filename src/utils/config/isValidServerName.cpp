
#include "UtilsConfig.hpp"

// TODO test this
bool isValidServerName(const std::string &str) {
	if (str.empty())
		return false;
	if (!isAlphaNum(str) && !isValidIpv4(str))
		return false;
	return true;
}
