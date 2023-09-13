
#include "UtilsConfig.hpp"

bool	isLetter(const std::string &str) {
	std::cout << "isLetter: " << str << std::endl; // ? debug
	for (std::string::const_iterator it = str.begin(); it != str.end(); it++) {
		if (!isalpha(*it))
			return (false);
	}
	return (true);
}
