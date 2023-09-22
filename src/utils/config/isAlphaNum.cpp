
#include "UtilsConfig.hpp"

// TODO fix this (added _ and . check for the server name)
bool	isAlphaNum(const std::string &str) {
	for (std::string::const_iterator it = str.begin(); it != str.end(); it++) {
		if (!isalnum(*it) && *it != '-' && *it != '.')
			return (false);
	}
	return (true);
}
