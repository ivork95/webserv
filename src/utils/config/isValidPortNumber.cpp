
#include "UtilsConfig.hpp"

// 0 to 65535
bool	isValidPortNumber(const std::string &portNumber) {
	if (!isNumber(portNumber))
		return (false);
	int	intPortNb = std::atoi(portNumber.c_str());
	if (intPortNb < 0 || intPortNb > 65535)
		return (false);
	return (true);	
}
