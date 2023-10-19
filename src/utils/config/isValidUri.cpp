
#include "UtilsConfig.hpp"

#include <filesystem>

/**
 * TODO valid URI ?
 * TODO		- starts with '/' ?
 * TODO		- ends with '/' ?
 * TODO		- contains only valid characters ? ...
 * TODO	is it a folder? file? none?
*/
bool	isValidUri(const std::string &str) {
	if (str.empty() || str.front() != '/')
		return false;
	if (str == "/")
		return true;
	return true;
}
