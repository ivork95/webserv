
#include "UtilsConfig.hpp"

// range from 0 to 65535
bool	isValidPortNumber(const std::string &str) {
	// std::cout << str.size() << std::endl; // ? debug
	if (str.size() > 5)
		return false;
	if (!isNumber(str))
		return false;
	const int	intPortNb = std::atoi(str.c_str());
	if (intPortNb < 0 || intPortNb > 65535)
		return false;
	return true;	
}
