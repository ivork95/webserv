
#include "UtilsConfig.hpp"

bool	hasConversionUnit(const std::string &str) {
	if (!str.empty()) {
		char lastChar = str.back();
		return (lastChar == 'k' || lastChar == 'K' || \
			lastChar == 'm' || lastChar == 'M' || \
			lastChar == 'g' || lastChar == 'G');
	}
	return (false);
}