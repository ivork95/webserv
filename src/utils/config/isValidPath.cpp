
#include "UtilsConfig.hpp"

bool	isValidPath(const std::string &str, const bool &isDirectory) {

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
