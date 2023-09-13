#ifndef UTILS_CONFIG_HPP
#define UTILS_CONFIG_HPP

# include <iostream>

bool	isNumber(const std::string &str);

bool	isHtmlExtension(const std::string &str);

bool	isPhpExtension(const std::string &str);

bool	isHttpMethod(const std::string &str);

bool	hasConversionUnit(const std::string &str);

bool	isValidErrorCode(const std::string &errorCode);

bool	isValidPortNumber(const std::string &portNumber);

bool	isValidServerName(const std::string &serverName);

bool	isLetter(const std::string &str);

bool	isAlphaNum(const std::string &str);

bool	isValidIpv4(const std::string &str);

bool	isValidCgiExtension(const std::string &cgiExtension);

bool	isValidPath(const std::string &filePath);

bool	isValidUri(const std::string &requestUri);

#endif
