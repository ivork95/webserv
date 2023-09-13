
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
 * ex of valid server names:
 * 	- localhost
 *  - example.com
 *  - www.example.com
 *  - 192.168.1.100
*/
bool isValidServerName(const std::string &serverName) {
	// std::cout << serverName << std::endl; // ? debug
	if (serverName.empty())
		return false;
	else if (serverName == "localhost" || serverName == "_")
		return true;
	else if (startsWithWww(serverName)) {
		// std::cout << "Check www." << std::endl; // ? debug
		if (endsWithCom(serverName)) {
			// std::cout << "Check .com" << std::endl; // ? debug
			std::string serverNameWithoutWwwCom = serverName.substr(4, serverName.size() - 8);
			if (!isAlphaNum(serverNameWithoutWwwCom))
				return false;
			return true;
		}
		std::string serverNameWithoutWww = serverName.substr(4);
		if (!isAlphaNum(serverNameWithoutWww))
			return false;
		return true;
	} else if (endsWithCom(serverName)) {
		// std::cout << "Check .com" << std::endl; // ? debug
		std::string serverNameWithoutCom = serverName.substr(0, serverName.size() - 4);
		if (!isAlphaNum(serverNameWithoutCom))
			return false;
		return true;
	} else if (isValidIpv4(serverName)) {
		return true;
	} else {
		return false;
	}
}
