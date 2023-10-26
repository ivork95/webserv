
#include "UtilsConfig.hpp"

bool	isValidCgiScript(const std::string &str) {
	// std::cout << "isValidCgiScript: " << str << std::endl; // ? debug

	const std::filesystem::path cgiScript(str);

	if (cgiScript.extension() != ".py")
		return false;
	if (!cgiScript.has_stem())
		return false;
	return true;
}
