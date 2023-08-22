
#ifndef ERROR_PAGE_HPP
# define ERROR_PAGE_HPP

# include <iostream>
# include <vector>

class ErrorPage {
	public:
		ErrorPage(void);
		ErrorPage(const std::string errorCode, const std::string filePath);
		~ErrorPage(void);

		std::vector<std::string>	errorCode;			// 404, 401, 403, 500
		std::string 				filePath;			// files/html/Website/Error/404.html

		void						printData(void);
};

#endif