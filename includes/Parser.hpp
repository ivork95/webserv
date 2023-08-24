#ifndef PARSER_HPP
#define PARSER_HPP

#include "Lexer.hpp"
#include "Configuration.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include <string.h>
#include <string>
#include <algorithm>
#include <vector>
#include <stack>

class Parser {
	public:
		Parser();
		~Parser(void);

		static Server	parseTokens(Server server);

	private:
		static void _identifyDirectives(Server *server, std::vector<Token> tokens);
		static void _parseListen(Server *server, std::vector<Token> tokens, size_t *i);
		static void _parseServerName(Server *server, std::vector<Token> tokens, size_t *i);
		static void _parseErrorPage(Server *server, std::vector<Token> tokens, size_t *i);
		static void _parseClientSize(Server *server, std::vector<Token> tokens, size_t *i);
		static void _parseLocation(Server *server, std::vector<Token> tokens, size_t *i);

	class InvalidTokenException : public std::exception {
		public:
			char const *what() const throw() {
				return ("Invalid token");
			}
	};
};

#endif
