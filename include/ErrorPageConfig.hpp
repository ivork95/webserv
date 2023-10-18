
#ifndef ERROR_PAGE_HPP
#define ERROR_PAGE_HPP

#include <iostream>
#include <vector>

class ErrorPageConfig
{
private:
    std::vector<int> _errorCodes;
    std::string _uriPath;

public:
    ErrorPageConfig(void);
    ErrorPageConfig(const std::vector<int> &errorCode, const std::string &uriPath);
    ~ErrorPageConfig(void);

    const std::vector<int> &getErrorCodes(void) const;
    const std::string &getUriPath(void) const;

    friend std::ostream &operator<<(std::ostream &out, const ErrorPageConfig &errorPage);
};

#endif
