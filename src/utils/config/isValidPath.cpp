
#include "UtilsConfig.hpp"

#include <filesystem>

/**
 * ? For some reason this does not work with "/" at the start => ugly workaround
*/
bool	isValidPath(const std::string &str, const bool &isDirectory) {
	// std::cout << "isValidPath: " << str << std::endl; // ? debug
	if (str.empty())
		return false;
	std::string inputPath = str;
	if (inputPath[0] == '/') {
		inputPath = inputPath.substr(1, inputPath.size());
	}
	if (isDirectory && !std::filesystem::is_directory(inputPath))
		return false;
	if (!isDirectory && !std::filesystem::is_regular_file(inputPath))
		return false;
	return true;
}
