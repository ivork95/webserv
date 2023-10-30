
#include "Parser.hpp"

#include <filesystem>

int convertToBytes(const std::string &rawValue) {
	std::string convertedValue{};
	for (size_t j = 0; j < rawValue.size() - 1; j++) {
		if (!isdigit(rawValue[j]))
			throw ClientMaxBodySizeException(rawValue);
		else
			convertedValue += rawValue[j];
	}

	if (convertedValue.empty())
		throw ClientMaxBodySizeException(convertedValue);

	const size_t lastCharIndex = rawValue.size() - 1;
	if (rawValue[lastCharIndex] == 'k' || rawValue[lastCharIndex] == 'K')
		convertedValue += "000";
	else if (rawValue[lastCharIndex] == 'm' || rawValue[lastCharIndex] == 'M')
		convertedValue += "000000";
	else if (rawValue[lastCharIndex] == 'g' || rawValue[lastCharIndex] == 'G')
		convertedValue += "000000000";
	
	int nb{};
	std::stringstream as;
	as.str(convertedValue);
	if (!(as >> nb))
		throw ClientMaxBodySizeException(rawValue);

	return nb;
}

int parseClientMaxBodySize(const std::string &rawValue) {
	if (rawValue == "0")
		return 0;
	else if (!hasConversionUnit(rawValue))
		throw ConversionUnitException(rawValue);
	else
		return (convertToBytes(rawValue));
}
