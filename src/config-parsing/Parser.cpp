
#include "Parser.hpp"

/**
 * CONSTRUCTORS / DESTRUCTORS
*/
Parser::Parser(void) {
	// std::cout << "Parser constructor called\n";
}

Parser::~Parser(void) {
	// std::cout << "Parser destructor called\n";
}

/**
 * STATIC FUNCTIONS
*/
static void	printCurrentToken(std::vector<Token> tokens, size_t i) {
	std::cout << "[" << i << "] " << tokens.at(i)._getWord() << " | " << tokens.at(i)._getType() << std::endl;
}

static std::string convertToBytes(const std::string &rawValue) {
	std::string convertedValue;
	for (size_t j = 0; j < rawValue.size() - 1; j++) {
		if (!isdigit(rawValue[j]))
			throw Parser::InvalidClientMaxBodySizeValueException();
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
	return (convertedValue);
}

/**
 * MEMBER FUNCTIONS
*/
/**
 * limit_except GET POST DELETE
 * limit_except <method> (<method> ...)
*/
void	Parser::_parseLimitExcept(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing limit_except directive\n";
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
		// TODO check if cgiExtension is valid
		if (tokens.at(*i + 1)._getType() == Token::WORD) {
			(*i)++;
			const std::string					cgiPath = tokens.at(*i)._getWord();
			// TODO check if cgiPath is valid
			std::map<std::string, std::string>	cgiHandler;
			cgiHandler[cgiExtension] = cgiPath;
			route.setCgiHandler(cgiExtension, cgiPath);
		} else {
			throw MissingFilePathException();
		}
	} else {
		throw ExpectedFileExtensionException();
	}
}

/**
 * index index.html index.php
 * index <file_name> (<file_name> ...)
*/
/**
 * TODO what extension can the files have ?
*/
void	Parser::_parseIndex(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing index directive\n";
	size_t						j = *i;
	std::vector<std::string>	indexFile;
	while (tokens.at(j)._getType() == Token::WORD) {
		if (isHtmlExtension(tokens.at(j)._getWord()) || isPhpExtension(tokens.at(j)._getWord())) {
			indexFile.push_back(tokens.at(j)._getWord());
			j++;
		} else {
			throw InvalidFileExtensionException();
		}
	}
	route.setIndexFile(indexFile);
	(*i) = j;
}

/**
 * autoindex on
 * autoindex <on/off>
*/
void	Parser::_parseAutoIndex(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing autoindex directive\n";
	if (tokens.at(*i)._getWord() == "off")
		route.setAutoIndex(false);
	else if (tokens.at(*i)._getWord() == "on")
		route.setAutoIndex(true);
	else
		throw InvalidAutoIndexValueException();
}

/**
 * client_max_body_size 1m
 * client_max_body_size <size> (where size is a number followed by k, m, or g)
*/
void	Parser::_parseClientMaxBodySize(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing client_max_body_size directive\n";
	const std::string rawValue = tokens.at(*i)._getWord();
	if (rawValue == "0")
		route.setClientMaxBodySize(rawValue);
	else if (!hasConversionUnit(rawValue))
		throw InvalidConversionUnitException();
	else {
		route.setClientMaxBodySize(convertToBytes(rawValue));
	}
}

/**
 * root /var/www/html
 * root <path_to_root>
*/
/**
 * TODO check if path_to_root is valid => if it exists and is a directory ?
*/
void	Parser::_parseRoot(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing root directive\n";
	route.setRootPath(tokens.at(*i)._getWord());
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
void Parser::_parseLocationContext(ServerConfig *server, std::vector<Token> tokens, size_t *i) {
	// std::cout << "Parsing location block\n";
	size_t			j = *i;
	std::string		requestURI = tokens.at(j)._getWord();
	LocationConfig	route(requestURI);

	j += 2; // skip opening brace

	void (Parser::*pf_locationContextDirectives[6])(std::vector<Token> tokens, size_t *j, LocationConfig &route) = {
		&Parser::_parseRoot, 
		&Parser::_parseClientMaxBodySize, 
		&Parser::_parseAutoIndex, 
		&Parser::_parseIndex, 
		&Parser::_parseCgi,
		&Parser::_parseLimitExcept
	};
	std::string locationContextDirectives[6] = {
		"root", 
		"client_max_body_size", 
		"autoindex", 
		"index", 
		"cgi",
		"limit_except"
	};

	while (tokens.at(j)._getType() != Token::CLOSE_BRACE) {
		for (size_t n = 0; n < 6; n++) {
			if (tokens.at(j)._getWord() == locationContextDirectives[n]) {
				j++;
				if (tokens.at(j)._getType() == Token::WORD) {
					(this->*pf_locationContextDirectives[n])(tokens, &j, route);
					break;
				} else {
					// TODO if we find the directive but nothing after => throw error or set default value ?
					throw ExpectedWordTokenException();
				}
			}
		}
		j++;
	}

	server->setLocationsConfig(route);
	(*i) = j;
}

/**
 * client_max_body_size 1m
 * client_max_body_size <size>
*/
/**
 * TODO valid size ? (only integers? max size?)
*/
void	Parser::_parseClientSize(ServerConfig *server, std::vector<Token> tokens, size_t *i) {
	const std::string rawValue = tokens.at(*i)._getWord();
	if (rawValue == "0")
		server->setClientMaxBodySize(rawValue);
	else if (!hasConversionUnit(rawValue))
		throw InvalidConversionUnitException();
	else {
		server->setClientMaxBodySize(convertToBytes(rawValue));
	}
}

/**
 * error_page 404 files/html/Website/Error/404.html
 * error_page <error_code (error_code ...)> <path_to_error_page>
*/
/**
 * TODO valid error code ? (400 to 599?)
*/
void	Parser::_parseErrorPage(ServerConfig *server, std::vector<Token> tokens, size_t *i) {
	// std::cout << "Parsing error_page directive\n";
	size_t						j = *i;
	std::vector<std::string>	errorCodes;
	while (tokens.at(j)._getType() == Token::WORD && isNumber(tokens.at(j)._getWord())) {
		const std::string	errorCode = tokens.at(j)._getWord();
		// TODO check if valid (// error code validity?)
		if (isValidErrorCode(errorCode)) {
			errorCodes.push_back(tokens.at(j)._getWord());
		} else {
			// throw InvalidErrorCodeException();
		}
		j++;
	}
	const std::string		filePath = tokens.at(j)._getWord();
	// TODO check if filePath is valid
	ErrorPageConfig	errorPage(errorCodes, filePath);
	server->setErrorPagesConfig(errorPage);
	(*i) = j;
}

/**
 * server_name localhost
 * server_name <server_name>
 * server_name <server_name> (<server_name> ...)
*/
/**
 * TODO valid server name ? localhost, example.com, www.example.com, 192.168.1.100
 * ? wip
*/
void	Parser::_parseServerName(ServerConfig *server, std::vector<Token> tokens, size_t *i) {
	// std::cout << "Parsing server_name directive\n";
	const std::string	serverName = tokens.at(*i)._getWord();
	if (!isValidServerName(serverName)) {
		std::cout << "Invalid server name: " << serverName << std::endl;
		throw InvalidServerNameException();
	}
	server->setServerName(serverName);
}

/**
 * listen 8080
 * listen <port_number>
*/
void	Parser::_parseListen(ServerConfig *server, std::vector<Token> tokens, size_t *i) {
	// std::cout << "Parsing listen directive\n";
	const std::string	portNumber = tokens.at(*i)._getWord();
	if (!isValidPortNumber(portNumber))
		throw InvalidPortNumberException();
	server->setPortNb(portNumber);
}

/**
 * Given the current token, loop over the directives and call the appropriate function
*/
void	Parser::_parseServerContext(ServerConfig *server, std::vector<Token> tokens, size_t *i) {
	void (Parser::*pf_serverContextDirectives[5])(ServerConfig *server, std::vector<Token> tokens, size_t *i) = {
		&Parser::_parseListen, 
		&Parser::_parseServerName, 
		&Parser::_parseErrorPage, 
		&Parser::_parseClientSize,
		&Parser::_parseLocationContext
	};
	std::string serverContextDirectives[5] = {
		"listen",
		"server_name", 
		"error_page", 
		"client_max_body_size",
		"location"
	};

	for (size_t n = 0; n < 5; n++) {
		if (tokens.at(*i)._getWord() == serverContextDirectives[n]) {
			(*i)++;
			if (tokens.at(*i)._getType() == Token::WORD) {
				(this->*pf_serverContextDirectives[n])(server, tokens, i);
				break;
			} else {
				// TODO if we find the directive but nothing after => throw error or set default value ?
				throw ExpectedWordTokenException();
			}
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
		_parseServerContext(server, tokens, &i);
		// try {
		// 	_parseServerContext(server, tokens, &i);
		// } catch (const std::exception &e) {
		// 	std::cerr << e.what() << std::endl;
		// 	// TODO set default value or throw error ?
		// }
	}
}

ServerConfig Parser::parseTokens(ServerConfig server) {
	Parser parser;

	// Identify directives
	// parser._identifyDirectives(&server, server.getTokens());

	try {
		parser._identifyDirectives(&server, server.getTokens());
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		// TODO set default value or throw error ?
	}

	// TODO check for N/A directives and set to default values or error ?

	return (server);
}
