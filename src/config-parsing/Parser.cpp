
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
void	Parser::_parseLocation(Server *server, std::vector<Token> tokens, size_t *i) {
	if (tokens.at(*i + 1)._getType() == Token::NA) {
		size_t		j = *i + 1;
		std::string	routePath = tokens.at(j)._getWord();
		Route 		route(routePath);
		j += 2; // skip opening brace
		while (tokens.at(j)._getType() != Token::CLOSE_BRACE) {
			if (tokens.at(j)._getWord() == "root") {
				std::string rootPath = tokens.at(j + 1)._getWord();
				route.rootPath = rootPath;
			} else if (tokens.at(j)._getWord() == "client_max_body_size") {
				std::string clientMaxBodySize = tokens.at(j + 1)._getWord();
				route.clientMaxBodySize = clientMaxBodySize;
			} else if (tokens.at(j)._getWord() == "autoindex") {
				bool autoIndex = false;
				if (tokens.at(j + 1)._getWord() == "off")
					autoIndex = false;
				else if (tokens.at(j + 1)._getWord() == "on")
					autoIndex = true;
				route.autoIndex = autoIndex;
			} else if (tokens.at(j)._getWord() == "index") {
				std::vector<std::string> indexFile;
				while (tokens.at(j + 1)._getType() == Token::NA && isHtmlExtension(tokens.at(j + 1)._getWord())) {
					indexFile.push_back(tokens.at(j + 1)._getWord());
					j++;
				}
				route.indexFile = indexFile;
			} else if (tokens.at(j)._getWord() == "cgi") {
				std::string cgiExtension;
				std::string cgiPath;
				std::map<std::string, std::string>	cgiHandler;
				if (tokens.at(j + 1)._getWord()[0] == '.') {
					cgiExtension = tokens.at(j + 1)._getWord();
					j++;
				}
				if (tokens.at(j + 1)._getType() == Token::NA) {
					cgiPath = tokens.at(j + 1)._getWord();
					j++;
				}
				route.cgiHandler[cgiExtension] = cgiPath;
			} else if (tokens.at(j)._getWord() == "limit_except") {
				std::vector<std::string> httpMethods;
				while (tokens.at(j + 1)._getType() == Token::NA && isHttpMethod(tokens.at(j + 1)._getWord())) {
					httpMethods.push_back(tokens.at(j + 1)._getWord());
					j++;
				}
				route.httpMethods = httpMethods;
			} else if (tokens.at(j)._getWord() == "}") {
				break;
			} else {
				// throw error
			}
			j++;
		}
		server->routesConfig.push_back(route);
		(*i) = j;
	} else {
		// throw error
	}
}

void	Parser::_parseClientSize(Server *server, std::vector<Token> tokens, size_t *i) {
	if (tokens.at(*i + 1)._getType() == Token::NA) {
		server->clientMaxBodySize = tokens.at(*i + 1)._getWord();
		(*i)++;
	} else {
		// throw error
	}
}

void	Parser::_parseErrorPage(Server *server, std::vector<Token> tokens, size_t *i) {
	if (tokens.at(*i + 1)._getType() == Token::NA) {
		size_t						j = *i + 1;
		std::vector<std::string>	errorCodes;
		while (tokens.at(j)._getType() == Token::NA && isNumber(tokens.at(j)._getWord())) {
			errorCodes.push_back(tokens.at(j)._getWord());
			j++;
		}
		std::string	filePath = tokens.at(j)._getWord();
		ErrorPage	errorPage(errorCodes, filePath);
		server->errorPages.push_back(errorPage);
		(*i) = j;
	} else {
		// throw error
	}
}

void	Parser::_parseServerName(Server *server, std::vector<Token> tokens, size_t *i) {
	if (tokens.at(*i + 1)._getType() == Token::NA) {
		server->serverName = tokens.at(*i + 1)._getWord();
		(*i)++;
	} else {
		// throw error
	}
}

void	Parser::_parseListen(Server *server, std::vector<Token> tokens, size_t *i) {
	if (tokens.at(*i + 1)._getType() == Token::NA) {
		server->portNb = tokens.at(*i + 1)._getWord();
		(*i)++;
	} else {
		// Next token is not a word
		throw InvalidTokenException();
	}
}

void	Parser::_identifyDirectives(Server *server, std::vector<Token> tokens) {
	for (size_t i = 0; i < tokens.size(); i++) {
		// std::cout << "[" << i << "] " << tokens.at(i)._getWord() << " | " << tokens.at(i)._getType() << std::endl;
		// server->printData(); // ? testing
		// Token::printTokens(&tokens); // ? testing
		if (tokens.at(i)._getType() == Token::NA) {
			if (tokens.at(i)._getWord() == "listen") {
				_parseListen(server, tokens, &i);
			} else if (tokens.at(i)._getWord() == "server_name") {
				_parseServerName(server, tokens, &i);
			} else if (tokens.at(i)._getWord() == "error_page") {
				_parseErrorPage(server, tokens, &i);
			} else if (tokens.at(i)._getWord() == "client_max_body_size") {
				_parseClientSize(server, tokens, &i);
			} else if (tokens.at(i)._getWord() == "location") {
				_parseLocation(server, tokens, &i);
			}
		}
	}
}

Server Parser::parseTokens(Server server) {
	// server.printData(); // ? testing

	// Identify directives
	_identifyDirectives(&server, server.tokens);

	// server.printData(); // ? testing

	// Validate directives

	// Build config objects (data structures)

	return server;
}
