
#include "UtilsConfig.hpp"

// TODO test this
bool isValidServerName(const std::string &str) {
	// std::cout << str << std::endl; // ? debug
	if (str.empty())
		return false;
	if (!isAlphaNum(str) && !isValidIpv4(str))
		return false;
	return true;
}
