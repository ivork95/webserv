
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
static void	printCurrentToken(std::vector<Token> tokens, size_t i) {
	std::cout << "[" << i << "] " << tokens.at(i)._getWord() << " | " << tokens.at(i)._getType() << std::endl;
}

void	Parser::_parseLocation(Server *server, std::vector<Token> tokens, size_t *i) {
	if (tokens.at(*i + 1)._getType() == Token::WORD) {
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
				while (tokens.at(j + 1)._getType() == Token::WORD && isHtmlExtension(tokens.at(j + 1)._getWord())) {
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
				if (tokens.at(j + 1)._getType() == Token::WORD) {
					cgiPath = tokens.at(j + 1)._getWord();
					j++;
				}
				route.cgiHandler[cgiExtension] = cgiPath;
			} else if (tokens.at(j)._getWord() == "limit_except") {
				std::vector<std::string> httpMethods;
				while (tokens.at(j + 1)._getType() == Token::WORD && isHttpMethod(tokens.at(j + 1)._getWord())) {
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
	if (tokens.at(*i + 1)._getType() == Token::WORD) {
		server->clientMaxBodySize = tokens.at(*i + 1)._getWord();
		(*i)++;
	} else {
		// throw error
	}
}

void	Parser::_parseErrorPage(Server *server, std::vector<Token> tokens, size_t *i) {
	if (tokens.at(*i + 1)._getType() == Token::WORD) {
		size_t						j = *i + 1;
		std::vector<std::string>	errorCodes;
		while (tokens.at(j)._getType() == Token::WORD && isNumber(tokens.at(j)._getWord())) {
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
	if (tokens.at(*i + 1)._getType() == Token::WORD) {
		server->serverName = tokens.at(*i + 1)._getWord();
		(*i)++;
	} else {
		// throw error
	}
}

void	Parser::_parseListen(Server *server, std::vector<Token> tokens, size_t *i) {
	if (tokens.at(*i + 1)._getType() == Token::WORD) {
		server->portNb = tokens.at(*i + 1)._getWord();
		(*i)++;
	} else {
		// Next token is not a word
		throw ExpectedWordTokenException();
	}
}

void	Parser::_parseDirective(Server *server, std::vector<Token> tokens, size_t *i) {
	void (Parser::*pf[5])(Server *server, std::vector<Token> tokens, size_t *i) = {
		&Parser::_parseListen, 
		&Parser::_parseServerName, 
		&Parser::_parseErrorPage, 
		&Parser::_parseClientSize, 
		&Parser::_parseLocation
	};
	std::string directive[5] = {
		"listen", 
		"server_name", 
		"error_page", 
		"client_max_body_size", 
		"location"
	};

	for (size_t j = 0; j < 5; j++) {
		if (tokens.at(*i)._getWord() == directive[j]) {
			(this->*pf[j])(server, tokens, i);
			break;
		}
	}
}

void	Parser::_identifyDirectives(Server *server, std::vector<Token> tokens) {
	for (size_t i = 0; i < tokens.size(); i++) {
		printCurrentToken(tokens, i);
		_parseDirective(server, tokens, &i);
	}
}

Server Parser::parseTokens(Server server) {
	Parser parser;
	
	// server.printData(); // ? testing

	// Identify directives
	parser._identifyDirectives(&server, server.tokens);

	// server.printData(); // ? testing

	// Validate directives

	// Build config objects (data structures)

	return server;
}
