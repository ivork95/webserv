
#include "Utils.hpp"

bool	isHttpMethod(const std::string &str) {
	if (str == "GET" || str == "POST")
		return true;
	return false;
}
