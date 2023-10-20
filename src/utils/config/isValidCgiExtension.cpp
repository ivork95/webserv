
#include "UtilsConfig.hpp"

bool	isValidCgiExtension(const std::string &str) {
	if (str.empty())
		return false;
	if (str != ".py" )
		return false;
	return true;
}
