
#include "UtilsConfig.hpp"

#include <filesystem>

/**
 * TODO valid URI ?
 * TODO		- starts with '/' ?
 * TODO		- ends with '/' ?
 * TODO		- contains only valid characters ? ...
 * TODO	if there is a root directive in the same location block, the URI is appended to the root
*/
// ! a URI is not necessarly a file, it could be a redirect to another location block
bool	isValidUri(const std::string &str) {
	// std::cout << "isValidUri: " << str << std::endl; // ? debug
	if (str.empty())
		return false;
	// if (!std::filesystem::is_regular_file(str) && !std::filesystem::is_directory(str)) {
	// 	std::string	rootPath;
	// 	if (str[0] == '/') {
	// 		rootPath = "www";
	// 	} else {
	// 		rootPath = "www/";
	// 	}
	// 	const std::string	fullPath = rootPath + str;
	// 	std::cout << "isValidUri fullPath: " << fullPath << std::endl; // ? debug
	// 	if (std::filesystem::is_regular_file(fullPath) || std::filesystem::is_directory(fullPath)) {
	// 		return true;
	// 	}
	// 	return false;
	// }
	return true;
}
