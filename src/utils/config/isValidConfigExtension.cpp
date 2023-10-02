
#include "UtilsConfig.hpp"

#include <filesystem>

// ! now static in InitConfig.cpp

bool	isValidConfigExtension(const std::string &str) {
	const std::string inputFileExtension = std::filesystem::path(str).extension();
	std::cout << inputFileExtension << std::endl; // ? debug

	if (std::filesystem::path(str).extension() != ".conf") {
		return false;
	}
	return true;
}
