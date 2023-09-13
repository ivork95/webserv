
#include "UtilsConfig.hpp"

/**
 * TODO same as isValidPath?
*/
bool	isValidUri(const std::string &requestUri) {
	if (requestUri.empty())
		return (false);
	if (requestUri[0] != '/')
		return (false);
	return (true);
}
