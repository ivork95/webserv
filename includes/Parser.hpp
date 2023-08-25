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
	private:
		void _identifyDirectives(Server *server, std::vector<Token> tokens);
		void _parseListen(Server *server, std::vector<Token> tokens, size_t *i);
		void _parseServerName(Server *server, std::vector<Token> tokens, size_t *i);
		void _parseErrorPage(Server *server, std::vector<Token> tokens, size_t *i);
		void _parseClientSize(Server *server, std::vector<Token> tokens, size_t *i);
		void _parseLocation(Server *server, std::vector<Token> tokens, size_t *i);
		void _parseDirective(Server *server, std::vector<Token> tokens, size_t *i);

	public:
		Parser(void);
		~Parser(void);


		static Server	parseTokens(Server server);

		class InvalidTokenException : public std::exception {
			public:
				char const* what() const throw() {
					return "Invalid token: ";
				}
		};

		class ExpectedWordTokenException : public InvalidTokenException {
			public:
				char const* what() const throw() {
					static std::string message = std::string(InvalidTokenException::what()) + "Expected word token";
					return message.c_str();
				}
		};
};

#endif
