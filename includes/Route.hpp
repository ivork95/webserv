
#ifndef ROUTE_HPP
# define ROUTE_HPP

# include <iostream>
# include <vector>
# include <map>

class Route {
	public:
		Route();
		Route(const std::string routePath);
		~Route(void);

		std::string 						requestURI;			// location / 
		std::string 						rootPath;			// root    files/html/Website
		std::string							clientMaxBodySize;	// client_max_body_size 42k
		bool								autoIndex;			// autoindex on/off
		std::vector<std::string>			indexFile;			// index   index.html Start.html
		std::map<std::string, std::string>	cgiHandler;			// cgi .php /usr/local/bin/php-cgi
		std::vector<std::string> 			httpMethods;		// methods GET POST DELETE

		void						printData(void);
};


#endif