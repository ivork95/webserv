#ifndef PARSER_HPP
#define PARSER_HPP

# include "Lexer.hpp"
# include "Configuration.hpp"
# include "Exceptions.hpp"
# include "Permissions.hpp"

# include <iostream>
# include <fstream>
# include <sstream>

# include <string.h>
# include <string>
# include <algorithm>
# include <vector>
# include <stack>

class Parser {
	private:
		void	_parseServerContext(ServerConfig *server, std::vector<Token> tokens, size_t *i);
		void	_parseListen(ServerConfig *server, std::vector<Token> tokens, size_t *i);
		void	_parseServerName(ServerConfig *server, std::vector<Token> tokens, size_t *i);
		void	_parseErrorPage(ServerConfig *server, std::vector<Token> tokens, size_t *i);
		void	_parseServerClientMaxBodySize(ServerConfig *server, std::vector<Token> tokens, size_t *i);

		void	_parseLocationContext(ServerConfig *server, std::vector<Token> tokens, size_t *i);
		void	_parseRoot(std::vector<Token> tokens, size_t *i, LocationConfig &route);
		void	_parseLocationClientMaxBodySize(std::vector<Token> tokens, size_t *i, LocationConfig &route);
		void	_parseIndex(std::vector<Token> tokens, size_t *i, LocationConfig &route);
		void	_parseAutoIndex(std::vector<Token> tokens, size_t *i, LocationConfig &route);
		void	_parseCgi(std::vector<Token> tokens, size_t *i, LocationConfig &route);
		void	_parseLimitExcept(std::vector<Token> tokens, size_t *i, LocationConfig &route);

	public:
		Parser(void);
		~Parser(void);

		static ServerConfig	parseTokens(ServerConfig server);

};

int convertToBytes(const std::string &rawValue);
int parseClientMaxBodySize(const std::string &rawValue);

#endif
