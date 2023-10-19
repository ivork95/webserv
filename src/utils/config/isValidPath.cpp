
#include "UtilsConfig.hpp"

#include <filesystem>

// bool	isValidPath(const std::string &str, const bool &isDirectory) {
// 	// if (isDirectory)
// 	// 	std::cout << "isValidPath (dir): " << str << std::endl; // ? debug
// 	// else
// 	// 	std::cout << "isValidPath (path): " << str << std::endl; // ? debug

// 	if (str.empty())
// 		return false;

// 	if (isDirectory && str == "/")
// 		return true;
	
// 	// Remove leading / 
// 	std::string inputPath = str;
// 	// std::cout << "Before leading /: " + inputPath << std::endl; // ? debug
// 	if (inputPath[0] == '/') {
// 		inputPath = inputPath.substr(1, inputPath.size());
// 		// std::cout << "Removed leading /: " + inputPath << std::endl; // ? debug
// 	}

// 	// std::cout << "Before checks: " + inputPath << std::endl; // ? debug

// 	if (isDirectory) {

// 		// Remove trailing /
// 		// std::cout << "Before trailing /: " + inputPath << std::endl; // ? debug
// 		if (inputPath[inputPath.size() - 1] == '/') {
// 			inputPath = inputPath.substr(0, inputPath.size() - 1);
// 			// std::cout << "Removed trailing /: " + inputPath << std::endl; // ? debug
// 		}

// 		if (std::filesystem::is_directory(inputPath)) {
// 			return true;
// 		}

// 		if (std::filesystem::is_directory("www/" + inputPath)) {
// 			return true;
// 		}

// 		return false;
// 	} else {
// 		if (std::filesystem::is_regular_file(inputPath))
// 			return true;
// 		return false;
// 	}
// }


bool	isValidPath(const std::string &str, const bool &isDirectory) {
	if (isDirectory)
		std::cout << "isValidPath (dir): " << str << std::endl; // ? debug
	else
		std::cout << "isValidPath (path): " << str << std::endl; // ? debug

	std::string inputPath = str;
	if (!isDirectory && !std::filesystem::is_regular_file(inputPath))
		return false;
	if (isDirectory && !std::filesystem::is_directory(inputPath))
		return false;
	return true;
}
