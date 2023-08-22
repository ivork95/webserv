
#ifndef ROUTE_HPP
# define ROUTE_HPP

# include <iostream>
# include <vector>

class Route {
	public:
		Route();
		Route(const std::string routePath);
		~Route(void);

		std::string 				routePath;			// location / 
		std::string 				rootDir;			// root    files/html/Website
		std::string					clientMaxBodySize;	// client_max_body_size 42k
		bool						autoIndex;			// autoindex on/off
		std::vector<std::string>	indexFile;			// index   index.html Start.html
		std::string 				cgiExtension;		// cgi .php /usr/local/bin/php-cgi
		std::vector<std::string> 	httpMethods;		// methods GET POST DELETE

		void						printData(void);
};


#endif