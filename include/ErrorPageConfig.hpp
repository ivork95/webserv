
#ifndef ERROR_PAGE_HPP
# define ERROR_PAGE_HPP

# include <iostream>
# include <vector>

class ErrorPageConfig {
	private:
		std::vector<std::string>	_errorCode;
		std::string 				_uriPath;

	public:
		ErrorPageConfig(void);
		ErrorPageConfig(const std::vector<std::string> &errorCode, const std::string &uriPath);
		~ErrorPageConfig(void);

		const std::vector<std::string>	&getErrorCode(void) const;
		const std::string				&getUriPath(void) const;

		friend std::ostream				&operator << (std::ostream &out, const ErrorPageConfig &errorPage);
};

#endif
