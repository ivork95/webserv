
#include "ParserHelper.hpp"

std::string ParserHelper::removeExtraSlashes(const std::string &path)
{
	size_t start = path.find_first_not_of('/');
	size_t end = path.find_last_not_of('/');

	if (start == std::string::npos || end == std::string::npos)
	{
		return "";
	}

	std::string cleanedPath = path.substr(start, end - start + 1);
	return cleanedPath;
}

bool ParserHelper::hasConversionUnit(const std::string &str)
{
	if (!str.empty())
	{
		char lastChar = str.back();
		return (lastChar == 'k' || lastChar == 'K' ||
				lastChar == 'm' || lastChar == 'M' ||
				lastChar == 'g' || lastChar == 'G');
	}
	return false;
}

bool ParserHelper::hasRequiredPermissions(const std::filesystem::path &filePath, std::filesystem::perms &requiredPermissions)
{
	std::filesystem::file_status status = std::filesystem::status(filePath);

	if (!std::filesystem::status_known(status) || (status.permissions() & requiredPermissions) != requiredPermissions)
		return false;
	return true;
}

bool ParserHelper::isAlphaNum(const std::string &str)
{
	for (std::string::const_iterator it = str.begin(); it != str.end(); it++)
	{
		if (!isalnum(*it) && *it != '-' && *it != '.')
			return (false);
	}
	return (true);
}

bool ParserHelper::isNumber(const std::string &str)
{
	for (std::string::const_iterator it = str.begin(); it != str.end(); it++)
	{
		if (!isdigit(*it))
			return false;
	}
	return true;
}

bool ParserHelper::isValidCgiInterpreter(const std::string &str)
{
	const std::filesystem::path cgiInterpreter(str);

	if (!std::filesystem::exists(cgiInterpreter))
		return false;
	if (!std::filesystem::is_regular_file(cgiInterpreter))
		return false;
	return true;
}

bool ParserHelper::isValidCgiScript(const std::string &str)
{
	const std::filesystem::path cgiScript(str);

	if (cgiScript.extension() != ".py")
		return false;
	if (!cgiScript.has_stem())
		return false;
	return true;
}

bool ParserHelper::isValidErrorCode(const int &errorCode)
{
	if (errorCode < 400 || errorCode > 599)
		return false;
	return true;
}

bool ParserHelper::isValidHttpMethod(const std::string &str)
{
	if (str.empty())
		return false;
	if (str == "GET" || str == "POST" || str == "DELETE")
		return true;
	return false;
}

bool ParserHelper::isValidIndexExtension(const std::string &str)
{
	if (str.empty())
		return false;

	const std::filesystem::path inputFile(str);

	if (inputFile.extension() != ".html")
		return false;
	return true;
}

bool ParserHelper::isValidPath(const std::string &str, const bool &isDirectory)
{
	if (str.empty())
		return false;
	if (!std::filesystem::exists(str))
		return false;
	if (isDirectory && !std::filesystem::is_directory(str))
		return false;
	if (!isDirectory && !std::filesystem::is_regular_file(str))
		return false;
	return true;
}

bool ParserHelper::isValidPortNumber(const std::string &str)
{
	if (str.size() > 5)
		return false;
	if (!isNumber(str))
		return false;
	const int intPortNb = std::atoi(str.c_str());
	if (intPortNb < 0 || intPortNb > 65535)
		return false;
	return true;
}

bool ParserHelper::isValidUri(const std::string &str)
{
	if (str.empty() || str.front() != '/')
		return false;
	if (str == "/")
		return true;
	return true;
}

static int countDots(const std::string &str)
{
	int dotCount = 0;
	for (char c : str)
	{
		if (c == '.')
		{
			dotCount++;
		}
	}
	return (dotCount);
}

bool ParserHelper::isValidIpv4(const std::string &str)
{
	if (str.empty())
		return false;
	if (!isdigit(str[0]))
		return false;

	std::vector<int> octets;
	std::istringstream iss(str);
	std::string octet;
	int dotCount = countDots(str);
	while (std::getline(iss, octet, '.'))
	{
		try
		{
			const int value = std::stoi(octet);
			if (value < 0 || value > 255)
				return false;
			octets.push_back(value);
		}
		catch (const std::invalid_argument &e)
		{
			return false;
		}
	}
	return (octets.size() == 4 && dotCount == 3);
}

bool ParserHelper::isValidServerName(const std::string &str)
{
	if (str.empty())
		return false;
	if (!isAlphaNum(str) && !isValidIpv4(str))
		return false;
	return true;
}

static int convertToBytes(const std::string &rawValue)
{
	std::string convertedValue{};
	for (size_t j = 0; j < rawValue.size() - 1; j++)
	{
		if (!isdigit(rawValue[j]))
			throw ClientMaxBodySizeException(rawValue);
		else
			convertedValue += rawValue[j];
	}

	if (convertedValue.empty())
		throw ClientMaxBodySizeException(convertedValue);

	const size_t lastCharIndex = rawValue.size() - 1;
	if (rawValue[lastCharIndex] == 'k' || rawValue[lastCharIndex] == 'K')
		convertedValue += "000";
	else if (rawValue[lastCharIndex] == 'm' || rawValue[lastCharIndex] == 'M')
		convertedValue += "000000";
	else if (rawValue[lastCharIndex] == 'g' || rawValue[lastCharIndex] == 'G')
		convertedValue += "000000000";

	int nb{};
	std::stringstream as;
	as.str(convertedValue);
	if (!(as >> nb))
		throw ClientMaxBodySizeException(rawValue);

	return nb;
}

int ParserHelper::parseClientMaxBodySize(const std::string &rawValue)
{
	if (rawValue == "0")
		return 0;
	else if (!hasConversionUnit(rawValue))
		throw ConversionUnitException(rawValue);
	else
		return (convertToBytes(rawValue));
}

int ParserHelper::openFile(std::ifstream *configFile, const std::string &filePath)
{
	configFile->open(filePath);
	if (!configFile->is_open())
	{
		std::cerr << "Error: Opening file: " << filePath << std::endl;
		return 1;
	}

	return 0;
}

bool ParserHelper::isValidConfigExtension(const std::string &str)
{
	const std::string inputFileExtension = std::filesystem::path(str).extension();

	if (inputFileExtension != ".conf")
	{
		std::cerr << "Error: Invalid config file extension: " << inputFileExtension << " (expected .conf)" << std::endl;
		return false;
	}

	return true;
}
