
#include "UtilsConfig.hpp"

bool	isValidPath(const std::string &str, const bool &isDirectory) {
	// std::cout << "isValidPath: " << str << std::endl; // ? debug

	if (str.empty())
		return false;
	if (!std::filesystem::exists(str))
		return false;
	if (isDirectory && !std::filesystem::is_directory(str))
		return false;
	if (!isDirectory && !std::filesystem::is_regular_file(str))
		return false;
	return true;
}
