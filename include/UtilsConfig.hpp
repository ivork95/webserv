#ifndef UTILS_CONFIG_HPP
#define UTILS_CONFIG_HPP

#include "Permissions.hpp"

#include <iostream>
#include <filesystem>

bool hasConversionUnit(const std::string &str);

bool hasRequiredPermissions(const std::filesystem::path &filePath, std::filesystem::perms &requiredPermissions);

bool isAlphaNum(const std::string &str);

bool isNumber(const std::string &str);

bool isValidCgiInterpreter(const std::string &str);

bool isValidCgiScript(const std::string &str);

bool isValidErrorCode(const int &errorCode);

bool isValidHttpMethod(const std::string &str);

bool isValidIndexExtension(const std::string &str);

bool isValidIpv4(const std::string &str);

bool isValidPath(const std::string &str, const bool &isDirectory);

bool isValidPortNumber(const std::string &str);

bool isValidServerName(const std::string &str);

bool isValidUri(const std::string &str);

#endif
