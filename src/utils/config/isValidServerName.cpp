
#include "UtilsConfig.hpp"

bool	startsWithWww(const std::string &str) {
	if (str[0] == 'w' && \
		str[1] == 'w' && \
		str[2] == 'w' && \
		str[3] == '.') {
		return true;
	}
	return false;
}

bool	endsWithCom(const std::string &str) {
	if (str[str.size() - 1] == 'm' && \
		str[str.size() - 2] == 'o' && \
		str[str.size() - 3] == 'c' && \
		str[str.size() - 4] == '.') {
		return true;
	}
	return false;
}

/**
 * ! can only contain digits, letters or hyphen
 * ? what if only "." "-" ...
*/
bool isValidServerName(const std::string &str) {
	// std::cout << str << std::endl; // ? debug
	if (str.empty())
		return false;
	if (!isAlphaNum(str))
		return false;
	if (!isValidIpv4(str))
		return true;
	return true;
}

/**
 * ex of valid server names:
 * 	- localhost
 *  - example.com
 *  - www.example.com
 *  - 192.168.1.100 (only localhost is accessible => 127.0.0.1.)
*/
// bool isValidServerName(const std::string &str) {
// 	// std::cout << str << std::endl; // ? debug
// 	if (str.empty())
// 		return false;
// 	else if (!isAlphaNum(str))
// 		return false;
// 	else if (str == "localhost" || str == "127.0.0.1.")
// 		return true;
// 	else if (startsWithWww(str)) {
// 		// std::cout << "Check www." << std::endl; // ? debug
// 		if (endsWithCom(str)) {
// 			// std::cout << "Check .com" << std::endl; // ? debug
// 			std::string serverNameWithoutWwwCom = str.substr(4, str.size() - 8);
// 			if (!isAlphaNum(serverNameWithoutWwwCom))
// 				return false;
// 			return true;
// 		}
// 		std::string serverNameWithoutWww = str.substr(4);
// 		if (!isAlphaNum(serverNameWithoutWww))
// 			return false;
// 		return true;
// 	} else if (endsWithCom(str)) {
// 		// std::cout << "Check .com" << std::endl; // ? debug
// 		std::string serverNameWithoutCom = str.substr(0, str.size() - 4);
// 		if (!isAlphaNum(serverNameWithoutCom))
// 			return false;
// 		return true;
// 	} else if (isValidIpv4(str)) {
// 		return true;
// 	} else {
// 		return false;
// 	}
// }
