
#include "UtilsConfig.hpp"

bool isHtmlExtension(const std::string &str) {
	std::string htmlExtension = ".html";
    if (str.length() >= htmlExtension.length()) {
        return (0 == str.compare(str.length() - htmlExtension.length(), htmlExtension.length(), htmlExtension));
    }
    return (false);
}

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
	const std::string fileExtension = str.substr(str.find_last_of(".") + 1);
	// std::cout << fileExtension << std::endl; // ? debug
	if (fileExtension == "html")
		return true;
	if (fileExtension == "php")
		return true;
	return false;
}
