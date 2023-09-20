
#ifndef LOCATION_CONFIG_HPP
# define LOCATION_CONFIG_HPP

# include "Exceptions.hpp"

# include <iostream>
# include <vector>
# include <map>

/**
 * TODO make derived class from server class?
*/
class LocationConfig {
	private:
		std::string 						_requestURI;		// location / 
		bool								_hasRequestURI{false};

		std::string 						_rootPath;			// root    files/html/Website
		bool								_hasRootPath{false};

		std::string							_clientMaxBodySize;	// client_max_body_size 42k
		bool								_hasClientMaxBodySize{false};

		bool								_autoIndex;			// autoindex on/off
		bool								_hasAutoIndex{false};

		std::vector<std::string>			_indexFile;			// index   index.html Start.html
		bool								_hasIndexFile{false};

		std::map<std::string, std::string>	_cgiHandler;		// cgi .php /usr/local/bin/php-cgi
		bool								_hasCgiHandler{false};

		std::vector<std::string> 			_httpMethods;		// methods GET POST DELETE
		bool								_hasHttpMethods{false};

	public:
		LocationConfig(void);
		LocationConfig(const std::string &requestURI);
		~LocationConfig(void);

		const std::string							&getRequestURI(void) const;
		const std::string							&getRootPath(void) const;
		const std::string							&getClientMaxBodySize(void) const;
		const bool									&getAutoIndex(void) const;
		const std::vector<std::string>				&getIndexFile(void) const;
		const std::map<std::string, std::string>	&getCgiHandler(void) const;
		const std::vector<std::string>				&getHttpMethods(void) const;

		void 										setRequestURI(const std::string &requestURI);
		void 										setRootPath(const std::string &rootPath);
		void 										setClientMaxBodySize(const std::string &clientMaxBodySize);
		void 										setAutoIndex(const bool &autoIndex);
		void 										setIndexFile(const std::vector<std::string> &indexFile);
		void 										setCgiHandler(const std::string &key, const std::string &value);
		void 										setHttpMethods(const std::vector<std::string> &httpMethods);

		bool										hasRequestURI(void) const;
		bool										hasRootPath(void) const;
		bool										hasClientMaxBodySize(void) const;
		bool										hasAutoIndex(void) const;
		bool										hasIndexFile(void) const;
		bool										hasCgiHandler(void) const;
		bool										hasHttpMethods(void) const;

		void										checkMissingDirective(LocationConfig &route);

		friend std::ostream							&operator << (std::ostream &out, const LocationConfig &route);
};


#endif