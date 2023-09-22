
#include "UtilsConfig.hpp"

/**
 * TODO same as isValidPath?
*/
bool	isValidUri(const std::string &str) {
	if (str.empty())
		return (false);
	if (str[0] != '/')
		return (false);
	return (true);
}
