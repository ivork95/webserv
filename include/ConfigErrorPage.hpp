
#ifndef CONFIG_ERROR_PAGE_HPP
#define CONFIG_ERROR_PAGE_HPP

#include <iostream>
#include <vector>

class ErrorPageConfig
{
private:
	std::vector<int> _errorCodes;
	std::string _filePath;

public:
	// default constructor
	ErrorPageConfig(void);

	// constructor
	ErrorPageConfig(const std::vector<int> &errorCode, const std::string &filePath);

	// getters/setters
	const std::vector<int> &getErrorCodes(void) const;
	const std::string &getFilePath(void) const;

	friend std::ostream &operator<<(std::ostream &out, const ErrorPageConfig &errorPage);
};

#endif