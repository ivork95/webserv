
#include "UtilsConfig.hpp"

#include <filesystem>

/**
 * ? For some reason this does not work with "/" at the start => ugly workaround
*/
bool	isValidPath(const std::string &str, const bool &isDirectory) {
	// std::cout << "isValidPath: " << str << std::endl; // ? debug
	if (str.empty())
		return false;
	if (str[0] == '/'){
		const std::string cleanedPath = str.substr(1, str.size());
		// std::cout << cleanedPath << std::endl; // ? debug
		if (isDirectory && !std::filesystem::is_directory(cleanedPath))
			return false;
		if (!isDirectory && !std::filesystem::is_regular_file(cleanedPath))
			return false;
		return true;
	}
	if (isDirectory && !std::filesystem::is_directory(str))
		return false;
	if (!isDirectory && !std::filesystem::is_regular_file(str))
		return false;
	return true;
}
