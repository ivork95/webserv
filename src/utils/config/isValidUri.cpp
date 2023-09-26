
#include "UtilsConfig.hpp"

#include <filesystem>

/**
 * TODO same as isValidPath?
*/
bool	isValidUri(const std::string &str) {
	// std::cout << "isValidPath: " << str << std::endl; // ? debug
	if (str.empty())
		return false;
	std::string	path = "www/" + str;
	std::cout << "isValidPath: " << path << std::endl; // ? debug
	// if (!std::filesystem::exists(str)) {
	// 	std::cout << "does not exist\n";
	// 	// return false;
	// }
	// if (std::filesystem::is_empty(str)) {
	// 	std::cout << "is empty\n";
	// 	// return false;
	// }
	if (!std::filesystem::is_regular_file(path) && !std::filesystem::is_directory(path)) {
		return false;
	}
	return (true);
}
