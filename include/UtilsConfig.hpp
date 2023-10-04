#ifndef UTILS_CONFIG_HPP
#define UTILS_CONFIG_HPP

# include <iostream>

bool	isNumber(const std::string &str);

bool	isHtmlExtension(const std::string &str);

bool	isPhpExtension(const std::string &str);

bool	hasConversionUnit(const std::string &str);

bool	isValidHttpMethod(const std::string &str);

bool	isValidErrorCode(const std::string &str);

bool	isValidPortNumber(const std::string &str);

bool	isValidServerName(const std::string &str);

bool	isAlphaNum(const std::string &str);

bool	isValidIpv4(const std::string &str);

bool	isValidCgiExtension(const std::string &str);

// bool	isValidConfigExtension(const std::string &str); // ! now static in InitConfig.cpp

bool	isValidIndexExtension(const std::string &str);

bool	isValidPath(const std::string &str, const bool &isDirectory);

bool	isValidUri(const std::string &str);

#endif
