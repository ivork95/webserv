
#include "UtilsConfig.hpp"

#include <filesystem>

// ! not used anymore
bool isHtmlExtension(const std::string &str) {
	std::string htmlExtension = ".html";
    if (str.length() >= htmlExtension.length()) {
        return (0 == str.compare(str.length() - htmlExtension.length(), htmlExtension.length(), htmlExtension));
    }
    return (false);
}

// ! not used anymore
bool isPhpExtension(const std::string &str) {
	std::string phpExtension = ".php";
	if (str.length() >= phpExtension.length()) {
		return (0 == str.compare(str.length() - phpExtension.length(), phpExtension.length(), phpExtension));
	}
	return (false);
}

/**
 * TODO valid index extension? html, php, txt, ... ?
 */
bool	isValidIndexExtension(const std::string &str) {
	const std::string inputFileExtension = std::filesystem::path(str).extension(); // TODO what if .php.php ?
	// std::cout << inputFileExtension << std::endl; // ? debug

	if (inputFileExtension != ".html" && inputFileExtension != ".php")
		return false;
	return true;
}
