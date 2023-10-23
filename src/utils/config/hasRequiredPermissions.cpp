#include "UtilsConfig.hpp"

bool hasRequiredPermissions(const std::string &str, std::filesystem::perms &requiredPermissions) {
	// std::cout << "hasRequiredPermissions: " << str << std::endl; // ? debug

	if (str.empty())
		return false;
	
	std::filesystem::path filePath = str;

	if (!std::filesystem::exists(filePath))
		return false;

	std::filesystem::file_status status = std::filesystem::status(filePath);

	if (!std::filesystem::status_known(status) || (status.permissions() & requiredPermissions) != requiredPermissions)
		return false;
	return true;
}
