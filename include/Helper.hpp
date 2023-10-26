#ifndef HELPER_HPP
#define HELPER_HPP

#include <vector>
#include <string>
#include <sstream>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <fcntl.h>

#include "StatusCodes.hpp"

class Helper
{
public:
    // constructor
    Helper(void) = delete;

    static char hexToChar(const std::string &hex);
    static int hexToInt(const std::string &hex);
    static std::string decodePercentEncoding(const std::string &encoded);
    static std::string percentEncode(const std::string &input);
    static void strip(std::string &str);
    static std::vector<std::string> split(const std::string &str);
    static std::string fileToStr(const std::string &path);
    static int setNonBlocking(int fd);
};

#endif