
#include "UtilsConfig.hpp"

#include <filesystem>

/**
 * TODO valid URI ?
 * TODO		- starts with '/' ?
 * TODO		- ends with '/' ?
 * TODO		- contains only valid characters ? ...
*/
// ! a URI is not necessarly a file, it could be a redirect to another location block
bool	isValidUri(const std::string &str) {
	// std::cout << "isValidUri: " << str << std::endl; // ? debug
	if (str.empty())
		return false;
	return true;
}
