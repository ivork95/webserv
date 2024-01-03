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

std::string Helper::fileToStr(const std::string &path)
{
    std::ifstream inf(path);

    if (!inf)
        throw StatusCodeException(404, "ifstream", errno);
    std::ostringstream sstr;
    sstr << inf.rdbuf();

    return sstr.str();
}

std::string Helper::percentEncode(const std::string &input)
{
    std::ostringstream encoded;

    for (char c : input)
    {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') // Unreserved characters in RFC 3986
            encoded << c;
        else
            encoded << '%' << std::uppercase << std::hex << ((c >> 4) & 0x0F) << (c & 0x0F);
    }

    return encoded.str();
}

int Helper::hexToInt(const std::string &hex)
{
    int value{};
    std::stringstream ss{};

    ss << std::hex << hex;
    ss >> value;

    return static_cast<int>(value);
}

int Helper::setNonBlocking(int fd)
{
    int result{};
    int flags{};

    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;
    flags |= O_NONBLOCK;
    result = fcntl(fd, F_SETFL, flags);
    if (result == -1)
        return -1;

    return 0;
}

bool Helper::isImageFormat(const std::string &methodPath)
{

    if (methodPath.ends_with("jpg") || methodPath.ends_with("jpeg") || methodPath.ends_with("png"))
        return true;

    return false;
}