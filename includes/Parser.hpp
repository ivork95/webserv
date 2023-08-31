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
		void _identifyDirectives(ServerConfig *server, std::vector<Token> tokens);

		void _parseDirective(ServerConfig *server, std::vector<Token> tokens, size_t *i);

		void _parseListen(ServerConfig *server, std::vector<Token> tokens, size_t *i);
		void _parseServerName(ServerConfig *server, std::vector<Token> tokens, size_t *i);
		void _parseErrorPage(ServerConfig *server, std::vector<Token> tokens, size_t *i);
		void _parseClientSize(ServerConfig *server, std::vector<Token> tokens, size_t *i);
		void _parseLocationBlock(ServerConfig *server, std::vector<Token> tokens, size_t *i);

		void _parseRoot(std::vector<Token> tokens, size_t *i, LocationConfig &route);
		void _parseClientMaxBodySize(std::vector<Token> tokens, size_t *i, LocationConfig &route);
		void _parseIndex(std::vector<Token> tokens, size_t *i, LocationConfig &route);
		void _parseAutoIndex(std::vector<Token> tokens, size_t *i, LocationConfig &route);
		void _parseCgi(std::vector<Token> tokens, size_t *i, LocationConfig &route);
		void _parseLimitExcept(std::vector<Token> tokens, size_t *i, LocationConfig &route);

	public:
		Parser(void);
		~Parser(void);

		static ServerConfig	parseTokens(ServerConfig server);

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

		class ExpectedHttpMethodTokenException : public InvalidTokenException {
			public:
				char const* what() const throw() {
					static std::string message = std::string(InvalidTokenException::what()) + "Expected HTTP method token (only GET, POST or DELETE)";
					return message.c_str();
				}
		};

		class MissingConversionUnit : public InvalidTokenException {
			public:
				char const* what() const throw() {
					static std::string message = std::string(InvalidTokenException::what()) + "Missing conversion unit (only 'k/K' or 'm/M' or 'g/G')";
					return message.c_str();
				}
		};

		class InvalidConversionUnit : public InvalidTokenException {
			public:
				char const* what() const throw() {
					static std::string message = std::string(InvalidTokenException::what()) + "Invalid conversion unit (only 'k/K', 'm/M' or 'g/G')";
					return message.c_str();
				}
		};

		class InvalidClientMaxBodySizeValueException : public InvalidTokenException {
			public:
				char const* what() const throw() {
					static std::string message = std::string(InvalidTokenException::what()) + "Invalid client_max_body_size value (only integers)";
					return message.c_str();
				}
		};

		class InvalidAutoIndexValueException : public InvalidTokenException {
			public:
				char const* what() const throw() {
					static std::string message = std::string(InvalidTokenException::what()) + "Invalid autoindex value (only 'on' or 'off')";
					return message.c_str();
				}
		};
};

#endif
