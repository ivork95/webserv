
#include "UtilsConfig.hpp"

bool hasRequiredPermissions(const std::filesystem::path &filePath, std::filesystem::perms &requiredPermissions) {
	// std::cout << "hasRequiredPermissions: " << filePath << std::endl; // ? debug

	std::filesystem::file_status status = std::filesystem::status(filePath);

	if (!std::filesystem::status_known(status) || (status.permissions() & requiredPermissions) != requiredPermissions)
		return false;
	return true;
}
