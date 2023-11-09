
#include "UtilsConfig.hpp"

bool	isValidPortNumber(const std::string &str) {

	if (str.size() > 5)
		return false;
	if (!isNumber(str))
		return false;
	const int	intPortNb = std::atoi(str.c_str());
	if (intPortNb < 0 || intPortNb > 65535)
		return false;
	return true;	
}
