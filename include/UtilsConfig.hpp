#ifndef UTILS_CONFIG_HPP
#define UTILS_CONFIG_HPP

# include <iostream>
# include <filesystem>

bool	isNumber(const std::string &str);


bool	hasConversionUnit(const std::string &str);

bool	isValidHttpMethod(const std::string &str);

bool	isValidErrorCode(const int &errorCode);

bool	isValidPortNumber(const std::string &str);

bool	isValidServerName(const std::string &str);

bool	isAlphaNum(const std::string &str);

bool	isValidIpv4(const std::string &str);

bool	isValidCgiExtension(const std::string &str);

bool	isValidIndexExtension(const std::string &str);

bool	isValidPath(const std::string &str, const bool &isDirectory);

bool	isValidUri(const std::string &str);

bool	hasRequiredPermissions(const std::string &str, std::filesystem::perms &requiredPermissions);

#endif
