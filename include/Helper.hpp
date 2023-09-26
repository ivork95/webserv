#ifndef HELPER_HPP
#define HELPER_HPP

#include <vector>
#include <string>
#include <sstream>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

class Helper
{
public:
    // constructor
    Helper(void) = delete;

    static char hexToChar(const std::string &hex);
    static std::string decodePercentEncoding(const std::string &encoded);
    static void strip(std::string &str);
    static std::vector<std::string> split(const std::string &str);
};

#endif