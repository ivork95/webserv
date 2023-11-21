#ifndef PARSER_HELPER_HPP
#define PARSER_HELPER_HPP

#include "Permissions.hpp"
#include "Exceptions.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include <filesystem>
#include <vector>

class ParserHelper
{
public:
	ParserHelper(void) = delete;

	static int openFile(std::ifstream *configFile, const std::string &filePath);

	static bool isValidConfigExtension(const std::string &str);

	static int parseClientMaxBodySize(const std::string &rawValue);

	static bool hasConversionUnit(const std::string &str);

	static bool hasRequiredPermissions(const std::filesystem::path &filePath, std::filesystem::perms &requiredPermissions);

	static bool isAlphaNum(const std::string &str);

	static bool isNumber(const std::string &str);

	static bool isValidCgiInterpreter(const std::string &str);

	static bool isValidCgiScript(const std::string &str);

	static bool isValidErrorCode(const int &errorCode);

	static bool isValidHttpMethod(const std::string &str);

	static bool isValidIndexExtension(const std::string &str);

	static bool isValidIpv4(const std::string &str);

	static bool isValidPath(const std::string &str, const bool &isDirectory);

	static bool isValidPortNumber(const std::string &str);

	static bool isValidServerName(const std::string &str);

	static bool isValidUri(const std::string &str);

	static std::string removeExtraSlashes(const std::string &path);
};

#endif