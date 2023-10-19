
#include "UtilsConfig.hpp"

#include <filesystem>

bool	isValidPath(const std::string &str, const bool &isDirectory) {
	if (str.empty())
		return false;
	if (!isDirectory && !std::filesystem::is_regular_file(str))
		return false;
	if (isDirectory && !std::filesystem::is_directory(str))
		return false;
	return true;
}
