
#include "Utils.hpp"

bool	hasConversionUnit(const std::string& str) {
    return str.find('k') != std::string::npos ||
           str.find('K') != std::string::npos ||
           str.find('m') != std::string::npos ||
           str.find('M') != std::string::npos ||
           str.find('g') != std::string::npos ||
           str.find('G') != std::string::npos;
}
