
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

// Logger logger("Parser");

/**
 * Member functions
*/
static void	printCurrentToken(std::vector<Token> tokens, size_t i) {
	std::cout << "[" << i << "] " << tokens.at(i)._getWord() << " | " << tokens.at(i)._getType() << std::endl;
}

/**
 * limit_except GET POST DELETE
 * limit_except <method> (<method> ...)
 * TODO error check method (can only be GET, POST, DELETE)
*/
void	Parser::_parseLimitExcept(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing limit_except directive\n";
	if (tokens.at(*i)._getType() == Token::WORD) {
		size_t						j = *i;
		std::vector<std::string>	httpMethods;
		while (tokens.at(j)._getType() == Token::WORD) {
			if (isHttpMethod(tokens.at(j)._getWord())) {
				httpMethods.push_back(tokens.at(j)._getWord());
				j++;
			} else {
				throw ExpectedHttpMethodTokenException();
			}
		}
		route.setHttpMethods(httpMethods);
		(*i) = j;
	} else {
		// throw error
	}
}

/**
 * cgi .php cgi-bin
 * cgi <file_extension> <path_to_cgi>
 * TODO 1. check file_extension (only file extensions ?)
 * TODO 2. check path_to_cgi
*/
void	Parser::_parseCgi(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing cgi directive\n";
	if (tokens.at(*i)._getWord()[0] == '.') {
		const std::string cgiExtension = tokens.at(*i)._getWord();
		if (tokens.at(*i + 1)._getType() == Token::WORD) {
			(*i)++;
			const std::string cgiPath = tokens.at(*i)._getWord();
			std::map<std::string, std::string> cgiHandler;
			cgiHandler[cgiExtension] = cgiPath;
			route.setCgiHandler(cgiExtension, cgiPath);
		} else {
			// throw error
		}
	} else {
		// throw error
	}
}

/**
 * index index.html index.php
 * index <file_name> (<file_name> ...)
*/
void	Parser::_parseIndex(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing index directive\n";
	if (tokens.at(*i)._getType() == Token::WORD) {
		size_t						j = *i;
		std::vector<std::string>	indexFile;
		while (tokens.at(j)._getType() == Token::WORD && isHtmlExtension(tokens.at(j)._getWord())) {
			indexFile.push_back(tokens.at(j)._getWord());
			j++;
		}
		route.setIndexFile(indexFile);
		(*i) = j;
	} else {
		// throw error
	}
}

/**
 * autoindex on
 * autoindex <on/off>
*/
void	Parser::_parseAutoIndex(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing autoindex directive\n";
	if (tokens.at(*i)._getType() == Token::WORD) {
		if (tokens.at(*i)._getWord() == "off")
			route.setAutoIndex(false);
		else if (tokens.at(*i)._getWord() == "on")
			route.setAutoIndex(true);
		else
			throw InvalidAutoIndexValueException();
	} else if (tokens.at(*i)._getType() == Token::SEMICOLON) {
		// set to default value (= off)
		route.setAutoIndex(false);
	} else {
		// throw error
	}
}

/**
 * client_max_body_size 1m
 * client_max_body_size <size> (where size is a number followed by k, m, or g)
 * TODO 1. check if size = 0
 * TODO 2. check if size has a conversion unit, and that it is at the end
 * TODO 3. convert size to bytes value
*/
void	Parser::_parseClientMaxBodySize(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing client_max_body_size directive\n";
	if (tokens.at(*i)._getType() == Token::WORD) {
		const std::string rawValue = tokens.at(*i)._getWord();
		if (rawValue == "0")
			route.setClientMaxBodySize(rawValue);
		else if (!hasConversionUnit(rawValue))
			throw InvalidConversionUnit();
		else {
			std::string convertedValue;
			for (size_t j = 0; j < rawValue.size() - 1; j++) {
				if (!isdigit(rawValue[j]))
					throw InvalidClientMaxBodySizeValueException();
				else
					convertedValue += rawValue[j];
			}
			const size_t lastCharIndex = rawValue.size() - 1;
			if (rawValue[lastCharIndex] == 'k' || rawValue[lastCharIndex] == 'K')
				convertedValue += "000";
			else if (rawValue[lastCharIndex] == 'm' || rawValue[lastCharIndex] == 'M')
				convertedValue += "000000";
			else if (rawValue[lastCharIndex] == 'g' || rawValue[lastCharIndex] == 'G')
				convertedValue += "000000000";
			route.setClientMaxBodySize(convertedValue);
		}
	} else {
		// throw error? set to default value (= 1m || 1000000)?
		route.setClientMaxBodySize("1000000");
	}
}

/**
 * root /var/www/html
 * root <path_to_root>
*/
void	Parser::_parseRoot(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing root directive\n";
	if (tokens.at(*i)._getType() == Token::WORD) {
		route.setRootPath(tokens.at(*i)._getWord());
	} else {
		// throw error
	}
}

/**
 * location / {
 *     root /var/www/html;
 *     index index.html index.php;
 *     autoindex on;
 *     cgi .php cgi-bin;
 *     limit_except GET POST;
 * }
 * location <requestURI> { ... }
*/
void Parser::_parseLocationBlock(ServerConfig *server, std::vector<Token> tokens, size_t *i) {
	// std::cout << "Parsing location block\n";
    if (tokens.at(*i)._getType() == Token::WORD) {
        size_t		j = *i;
        std::string	requestURI = tokens.at(j)._getWord();
        LocationConfig		route(requestURI);

        j += 2; // skip opening brace

		void (Parser::*pf_blockDirective[6])(std::vector<Token> tokens, size_t *j, LocationConfig &route) = {
			&Parser::_parseRoot, 
			&Parser::_parseClientMaxBodySize, 
			&Parser::_parseAutoIndex, 
			&Parser::_parseIndex, 
			&Parser::_parseCgi,
			&Parser::_parseLimitExcept
		};
		std::string directive[6] = {
			"root", 
			"client_max_body_size", 
			"autoindex", 
			"index", 
			"cgi",
			"limit_except"
		};

		while (tokens.at(j)._getType() != Token::CLOSE_BRACE) {
			for (size_t n = 0; n < 6; n++) {
				if (tokens.at(j)._getWord() == directive[n]) {
					j++;
					(this->*pf_blockDirective[n])(tokens, &j, route);
					break;
				}
			}
			j++;
		}

		server->setRoutesConfig(route);
		(*i) = j;
    } else {
        // throw error
    }
}

/**
 * client_max_body_size 1m
 * client_max_body_size <size>
*/
void	Parser::_parseClientSize(ServerConfig *server, std::vector<Token> tokens, size_t *i) {
	if (tokens.at(*i)._getType() == Token::WORD) {
		const std::string rawValue = tokens.at(*i)._getWord();
		if (rawValue == "0")
			server->setClientMaxBodySize(rawValue);
		else if (!hasConversionUnit(rawValue))
			throw InvalidConversionUnit();
		else {
			// TODO make this a function
			std::string convertedValue;
			for (size_t j = 0; j < rawValue.size() - 1; j++) {
				if (!isdigit(rawValue[j]))
					throw InvalidClientMaxBodySizeValueException();
				else
					convertedValue += rawValue[j];
			}
			const size_t lastCharIndex = rawValue.size() - 1;
			if (rawValue[lastCharIndex] == 'k' || rawValue[lastCharIndex] == 'K')
				convertedValue += "000";
			else if (rawValue[lastCharIndex] == 'm' || rawValue[lastCharIndex] == 'M')
				convertedValue += "000000";
			else if (rawValue[lastCharIndex] == 'g' || rawValue[lastCharIndex] == 'G')
				convertedValue += "000000000";
			server->setClientMaxBodySize(convertedValue);
		}
	} else if (tokens.at(*i)._getType() == Token::SEMICOLON) {
		throw ExpectedWordTokenException();
	} else {
		// throw error? set to default value (= 1m || 1000000)?
		server->setClientMaxBodySize("1000000");
	}
}

/**
 * error_page 404 files/html/Website/Error/404.html
 * error_page <error_code (error_code ...)> <path_to_error_page>
*/
void	Parser::_parseErrorPage(ServerConfig *server, std::vector<Token> tokens, size_t *i) {
	// std::cout << "Parsing error_page directive\n";
	if (tokens.at(*i)._getType() == Token::WORD) {
		size_t						j = *i;
		std::vector<std::string>	errorCodes;
		while (tokens.at(j)._getType() == Token::WORD && isNumber(tokens.at(j)._getWord())) {
			errorCodes.push_back(tokens.at(j)._getWord());
			j++;
		}
		std::string	filePath = tokens.at(j)._getWord();
		ErrorPageConfig	errorPage(errorCodes, filePath);
		server->setErrorPages(errorPage);
		(*i) = j;
	} else {
		// throw error
	}
}

/**
 * server_name localhost
 * server_name <server_name>
*/
void	Parser::_parseServerName(ServerConfig *server, std::vector<Token> tokens, size_t *i) {
	// std::cout << "Parsing server_name directive\n";
	if (tokens.at(*i)._getType() == Token::WORD) {
		server->setServerName(tokens.at(*i)._getWord());
	} else {
		// throw error
	}
}

/**
 * listen 8080
 * listen <port_number>
*/
void	Parser::_parseListen(ServerConfig *server, std::vector<Token> tokens, size_t *i) {
	// std::cout << "Parsing listen directive\n";
	if (tokens.at(*i)._getType() == Token::WORD) {
		server->setPortNb(tokens.at(*i)._getWord());
	} else {
		// Next token is not a word
		throw ExpectedWordTokenException();
	}
}

/**
 * Given the current token, loop over the directives and call the appropriate function
*/
void	Parser::_parseDirective(ServerConfig *server, std::vector<Token> tokens, size_t *i) {
	void (Parser::*pf_simpleDirective[4])(ServerConfig *server, std::vector<Token> tokens, size_t *i) = {
		&Parser::_parseListen, 
		&Parser::_parseServerName, 
		&Parser::_parseErrorPage, 
		&Parser::_parseClientSize, 
	};
	std::string simpleDirective[4] = {
		"listen",
		"server_name", 
		"error_page", 
		"client_max_body_size", 
	};

	for (size_t n = 0; n < 4; n++) {
		if (tokens.at(*i)._getWord() == simpleDirective[n]) {
			(*i)++;
			(this->*pf_simpleDirective[n])(server, tokens, i);
			break;
		} else if (tokens.at(*i)._getWord() == "location") {
			(*i)++;
			_parseLocationBlock(server, tokens, i);
			break;
		}
	}
}

/**
 * Loop over tokens and identify directives (listen, server_name, error_page, client_max_body_size, location)
 * ? should I init every directive to default values 
 * ? in case they are not found in the config file ?
*/
void	Parser::_identifyDirectives(ServerConfig *server, std::vector<Token> tokens) {
	for (size_t i = 0; i < tokens.size(); i++) {
		// printCurrentToken(tokens, i); // ? testing
		_parseDirective(server, tokens, &i);
	}
}

ServerConfig Parser::parseTokens(ServerConfig server) {
	Parser parser;

	// Identify directives
	parser._identifyDirectives(&server, server.getTokens());

	// TODO check for N/A directives and set to default values or error ?

	return server;
}
