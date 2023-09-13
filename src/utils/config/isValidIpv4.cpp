
#include "UtilsConfig.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

int	countDots(const std::string &str) {
	int	dotCount = 0;
	for (char c : str) {
		if (c == '.') {
			dotCount++;
		}
	}
	return (dotCount);
}

/**
 * valid IP address:
 * 	X.X.X.X
 * 		where X is a number from 0 to 255
 * 		=> 4 numbers (octets) and 3 dots
*/
bool	isValidIpv4(const std::string &str) {
	if (str.empty())
		return false;
	if (!isdigit(str[0])) {
		return false;
	}
    std::vector<int>	octets;
    std::istringstream	iss(str);
    std::string			octet;
	int					dotCount = countDots(str);
    while (std::getline(iss, octet, '.')) {
		// std::cout << "octet: " << octet << std::endl; // ? debug
        try {
            int value = std::stoi(octet);
            if (value < 0 || value > 255) {
                return false;
            }
            octets.push_back(value);
        } catch (const std::invalid_argument &e) {
            return false;
        }
    }
    return (octets.size() == 4 && dotCount == 3);
}
