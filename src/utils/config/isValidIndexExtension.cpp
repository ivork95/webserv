
#include "UtilsConfig.hpp"

bool	isValidIndexExtension(const std::string &str) {
	// std::cout << "isValidIndexExtension: " + str << std::endl; // ? debug

	if (str.empty())
		return false;

	const std::filesystem::path inputFile(str);

	if (inputFile.extension() != ".html")
		return false;
	return true;
}
