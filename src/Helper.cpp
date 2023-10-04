#include "Helper.hpp"

char Helper::hexToChar(const std::string &hex)
{
    int value{};
    std::stringstream ss{};

    ss << std::hex << hex;
    ss >> value;
    return static_cast<char>(value);
}

std::string Helper::decodePercentEncoding(const std::string &encoded)
{
    std::string decoded{};

    for (size_t i = 0; i < encoded.length(); ++i)
    {
        if (encoded[i] == '%' && i + 2 < encoded.length())
        {
            std::string hexValue = encoded.substr(i + 1, 2);
            decoded += hexToChar(hexValue);
            i += 2; // skip the next two characters
        }
        else
            decoded += encoded[i];
    }
    spdlog::critical("encoded = {}", encoded);
    spdlog::critical("decoded = {}", decoded);
    return decoded;
}

void Helper::strip(std::string &str)
{
    str.erase(std::remove(str.begin(), str.end(), '\"'), str.end());
}

std::vector<std::string> Helper::split(const std::string &str)
{
    std::vector<std::string> methodPathVersion;
    std::istringstream iss(str);
    std::string keyword;

    while (getline(iss, keyword, ' '))
        methodPathVersion.push_back(keyword);

    return methodPathVersion;
}