
#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <vector>

# include "Token.hpp"
# include "Route.hpp"
# include "ErrorPage.hpp"

class Server {
	public:
		Server(void);
		Server(const unsigned int index, const std::string rawData);
		~Server(void);

		unsigned int				_index;

		std::string					portNb;				// listen  8080
		std::string					serverName;			// server_name localhost
		std::vector<ErrorPage>		errorPages;			// error_page 404 files/html/Website/Error/404.html
		std::vector<Route>			routesConfig;		// location / { ... }, location /upload { ... }, location /test { ... }
		std::string					clientMaxBodySize;	// client_max_body_size 10M

		std::string					rawData;
		std::vector<Token>			tokens;

		void						printData(void);
};

#endif