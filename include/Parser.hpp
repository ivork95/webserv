#ifndef PARSER_HPP
#define PARSER_HPP

#include "Lexer.hpp"
#include "Configuration.hpp"
#include "Exceptions.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include <string.h>
#include <string>
#include <algorithm>
#include <vector>
#include <stack>

#define OWREAD std::filesystem::perms::owner_read
#define OWWRITE std::filesystem::perms::owner_write
#define OWEXEC std::filesystem::perms::owner_exec

#define OTREAD std::filesystem::perms::others_read
#define OTWRITE std::filesystem::perms::others_write
#define OTEXEC std::filesystem::perms::others_exec

#define GRREAD std::filesystem::perms::group_read
#define GRWRITE std::filesystem::perms::group_write
#define GREXEC std::filesystem::perms::group_exec

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

#endif
