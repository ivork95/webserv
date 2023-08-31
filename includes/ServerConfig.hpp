
#ifndef SERVER_CONFIG_HPP
# define SERVER_CONFIG_HPP

# include <iostream>
# include <vector>

# include "Token.hpp"
# include "LocationConfig.hpp"
# include "ErrorPageConfig.hpp"

class ServerConfig {
	private:
		unsigned int					_index;

		std::string						_portNb;				// listen  8080
		std::string						_serverName;			// server_name localhost
		std::string						_clientMaxBodySize;		// client_max_body_size 10M
		std::vector<ErrorPageConfig>	_errorPagesConfig;			// error_page 404 405 406 files/html/Website/Error/404.html
		std::vector<LocationConfig>		_locationsConfig;			// location / { ... }, location /upload { ... }, location /test { ... }

		std::string						_rawData;
		std::vector<Token>				_tokens;

	public:
		ServerConfig(void);
		ServerConfig(const unsigned int index, const std::string rawData);
		~ServerConfig(void);

		const unsigned int					&getIndex(void) const;
		const std::string					&getPortNb(void) const;
		const std::string					&getServerName(void) const;
		const std::string					&getClientMaxBodySize(void) const;
		const std::vector<ErrorPageConfig>	&getErrorPagesConfig(void) const;
		const std::vector<LocationConfig>	&getLocationsConfig(void) const;
		const std::string					&getRawData(void) const;
		const std::vector<Token>			&getTokens(void) const;

		void							setIndex(const unsigned int index);
		void							setPortNb(const std::string &portNb);
		void							setServerName(const std::string &serverName);
		void							setClientMaxBodySize(const std::string &clientMaxBodySize);
		void							setErrorPagesConfig(const ErrorPageConfig &errorPagesConfig);
		void							setLocationsConfig(const LocationConfig &locationsConfig);
		void							setRawData(const std::string &rawData);
		void							setTokens(const std::vector<Token> &tokens);

		friend std::ostream				&operator << (std::ostream &out, const ServerConfig &server);
};

#endif