
#include "Parser.hpp"

/**
 * Constructors / destructors
*/
Parser::Parser(void) {
	// std::cout << "Parser constructor called\n";
}

Parser::~Parser(void) {
	// std::cout << "Parser destructor called\n";
}

/**
 * Member functions
*/
void Parser::_identifyDirectives(Server *server, std::vector<Token> tokens) {
	for (size_t i = 0; i < tokens.size(); i++) {
		if (tokens.at(i)._getType() == Token::NA) {
			if (tokens.at(i)._getWord() == "listen") {
				if (tokens.at(i + 1)._getType() == Token::NA) {
					server->portNb = tokens.at(i + 1)._getWord();
					i++;
				} else {
					// throw error
				}
			} else if (tokens.at(i)._getWord() == "server_name") {
				if (tokens.at(i + 1)._getType() == Token::NA) {
					server->serverName = tokens.at(i + 1)._getWord();
					i++;
				} else {
					// throw error
				}
			} else if (tokens.at(i)._getWord() == "error_page") {
				if (tokens.at(i + 1)._getType() == Token::NA) {
					ErrorPage errorPage(tokens.at(i + 1)._getWord(), tokens.at(i + 2)._getWord());
					// errorPage.printData();
					server->errorPages.push_back(errorPage);
					i++;
				} else {
					// throw error
				}
			} else if (tokens.at(i)._getWord() == "client_max_body_size") {
				if (tokens.at(i + 1)._getType() == Token::NA) {
					server->clientMaxBodySize = tokens.at(i + 1)._getWord();
					i++;
				} else {
					// throw error
				}
			} else if (tokens.at(i)._getWord() == "location") {
				if (tokens.at(i + 1)._getType() == Token::NA) {
					Route route(tokens.at(i + 1)._getWord());
					server->routesConfig.push_back(route);
					i++;
				} else {
					// throw error
				}
			}
		}
	}
}

Server Parser::parseTokens(Server server) {
	Logger logger("Parser::parseTokens");

	// Token::printTokens(tokens); // ? testing

	// Identify directives
	_identifyDirectives(&server, server.tokens);

	// server.printData(); // ? testing

	// Validate directives

	// Build config objects (data structures)

	return server;
}
