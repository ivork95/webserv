
#include "UtilsConfig.hpp"

bool	isNumber(std::string str) {
	for (std::string::iterator it = str.begin(); it != str.end(); it++) {
		if (!isdigit(*it))
			return (false);
	}
	return (true);
}
