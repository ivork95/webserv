
#ifndef ERROR_PAGE_HPP
# define ERROR_PAGE_HPP

# include <iostream>
# include <vector>

class ErrorPage {
	private:
		std::vector<std::string>	_errorCode;			// 404, 401, 403, 500
		std::string 				_filePath;			// files/html/Website/Error/404.html

	public:
		ErrorPage(void);
		ErrorPage(const std::vector<std::string> &errorCode, const std::string &filePath);
		~ErrorPage(void);

		std::vector<std::string>	getErrorCode(void) const;
		std::string					getFilePath(void) const;

		friend std::ostream			&operator << (std::ostream &out, const ErrorPage &errorPage);

		// void						printData(void);
};

#endif
