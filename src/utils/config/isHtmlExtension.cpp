
#include "UtilsConfig.hpp"

bool isHtmlExtension(const std::string &str) {
	std::string htmlExtension = ".html";
    if (str.length() >= htmlExtension.length()) {
        return (0 == str.compare(str.length() - htmlExtension.length(), htmlExtension.length(), htmlExtension));
    }
    return false;
}
