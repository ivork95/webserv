
#ifndef ROUTE_HPP
# define ROUTE_HPP

# include <iostream>
# include <vector>
# include <map>

class Route {
	private:
		std::string 						_requestURI;			// location / 
		std::string 						_rootPath;			// root    files/html/Website
		std::string							_clientMaxBodySize;	// client_max_body_size 42k
		bool								_autoIndex;			// autoindex on/off
		std::vector<std::string>			_indexFile;			// index   index.html Start.html
		std::map<std::string, std::string>	_cgiHandler;			// cgi .php /usr/local/bin/php-cgi
		std::vector<std::string> 			_httpMethods;		// methods GET POST DELETE

	public:
		Route(void);
		Route(const std::string routePath);
		~Route(void);

		std::string							getRequestURI(void) const;
		std::string							getRootPath(void) const;
		std::string							getClientMaxBodySize(void) const;
		bool								getAutoIndex(void) const;
		std::vector<std::string>			getIndexFile(void) const;
		std::map<std::string, std::string>	getCgiHandler(void) const;
		std::vector<std::string>			getHttpMethods(void) const;

		friend std::ostream			&operator << (std::ostream &out, const Route &route);

		// void						printData(void);
};


#endif