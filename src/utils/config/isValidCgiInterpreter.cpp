
#include "UtilsConfig.hpp"

bool	isValidCgiInterpreter(const std::string &str) {
	// std::cout << "isValidCgiInterpreter: " << str << std::endl; // ? debug

	const std::filesystem::path cgiInterpreter(str);

	if (!std::filesystem::exists(cgiInterpreter))
		return false;
	if (!std::filesystem::is_regular_file(cgiInterpreter))
		return false;
	return true;
}