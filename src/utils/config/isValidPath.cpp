
#include "UtilsConfig.hpp"

/**
 * TODO check path access?
*/
bool	isValidPath(const std::string &filePath) {
	if (filePath.empty())
		return false;
	// if (filePath[0] != '/')
	// 	return false;
	return true;
}
