
#include "UtilsConfig.hpp"

#include <filesystem>

/**
 * TODO check path access? 
*/
bool	isValidPath(const std::string &str) {
	// std::cout << "isValidPath: " << str << std::endl; // ? debug
	// std::string path = "www/" + str;
	// std::cout << "isValidPath: " << path << std::endl; // ? debug
	if (str.empty())
		return false;
	// if (!std::filesystem::exists(str)) {
	// 	std::cout << "does not exist\n";
	// 	// return false;
	// }
	// if (std::filesystem::is_empty(str)) {
	// 	std::cout << "is empty\n";
	// 	// return false;
	// }
	if (!std::filesystem::is_regular_file(str) && !std::filesystem::is_directory(str)) {
		return false;
	}
	return true;
}
