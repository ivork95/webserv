
#include "UtilsConfig.hpp"

#include <filesystem>

bool	isValidIndexExtension(const std::string &str) {
	const std::string inputFileExtension = std::filesystem::path(str).extension();
	// std::cout << inputFileExtension << std::endl; // ? debug

	if (inputFileExtension.size() > 0)
		return true;
	if (inputFileExtension != ".html")
		return false;
	return true;
}
