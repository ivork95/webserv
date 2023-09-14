
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
	std::cout << "[" << i << "] " << tokens.at(i) << std::endl;
}

static std::string convertToBytes(const std::string &rawValue) {
	std::string convertedValue;
	for (size_t j = 0; j < rawValue.size() - 1; j++) {
		if (!isdigit(rawValue[j]))
			throw ClientMaxBodySizeException(rawValue);
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

static const std::string parseClientMaxBodySize(const std::string &rawValue) {
	if (rawValue == "0")
		return rawValue;
	else if (!hasConversionUnit(rawValue))
		throw ConversionUnitException(rawValue);
	else
		return (convertToBytes(rawValue));
}

/**
 * MEMBER FUNCTIONS
*/
/**
 * limit_except GET POST DELETE
 * limit_except <method> (<method> ...)
*/
void	Parser::_parseLimitExcept(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing limit_except directive\n"; // ? debug
	size_t						j = *i;
	std::vector<std::string>	httpMethods;
	while (tokens.at(j)._getType() == Token::WORD) {
		if (isValidHttpMethod(tokens.at(j)._getWord())) {
			httpMethods.push_back(tokens.at(j)._getWord());
			j++;
		} else {
			throw HttpMethodException(tokens.at(j)._getWord());
		}
	}
	route.setHttpMethods(httpMethods);
	(*i) = j;
}

/**
 * cgi .php cgi-bin
 * cgi <file_extension> <path_to_cgi>
 * TODO 1. check file_extension (only file extensions ?) 
 * 			? wip
 * TODO 2. check path_to_cgi
 * 			? wip
*/
void	Parser::_parseCgi(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing cgi directive\n"; // ? debug
	if (tokens.at(*i)._getWord()[0] == '.') {
		const std::string cgiExtension = tokens.at(*i)._getWord();
		if (!isValidCgiExtension(cgiExtension))
			throw CgiExtensionException(cgiExtension);
		if (tokens.at(*i + 1)._getType() == Token::WORD) {
			(*i)++;
			const std::string					cgiPath = tokens.at(*i)._getWord();
			if (!isValidPath(cgiPath))
				throw PathException(cgiPath);
			std::map<std::string, std::string>	cgiHandler;
			cgiHandler[cgiExtension] = cgiPath;
			route.setCgiHandler(cgiExtension, cgiPath);
		} else {
			throw InvalidTokenException("Missing file path");
		}
	} else {
		throw InvalidTokenException("Expected file extension");
	}
}

/**
 * index index.html index.php
 * index <file_name> (<file_name> ...)
*/
/**
 * TODO what extension can the files have ? html, php, txt, ... ?
 * 		? wip
*/
void	Parser::_parseIndex(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing index directive\n"; // ? debug
	size_t						j = *i;
	std::vector<std::string>	indexFile;
	while (tokens.at(j)._getType() == Token::WORD) {
		if (isValidIndexExtension(tokens.at(j)._getWord())) {
			indexFile.push_back(tokens.at(j)._getWord());
			j++;
		} else {
			throw IndexException(tokens.at(j)._getWord()); // TODO change?
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
	// std::cout << "\tParsing autoindex directive\n"; // ? debug
	if (tokens.at(*i)._getWord() == "off")
		route.setAutoIndex(false);
	else if (tokens.at(*i)._getWord() == "on")
		route.setAutoIndex(true);
	else
		throw AutoIndexException(tokens.at(*i)._getWord());
}

/**
 * client_max_body_size 1m
 * client_max_body_size <size> (where size is a number followed by k, m, or g)
*/
void	Parser::_parseLocationClientMaxBodySize(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing client_max_body_size directive\n"; // ? debug
	const std::string rawValue = tokens.at(*i)._getWord();
	route.setClientMaxBodySize(parseClientMaxBodySize(rawValue));
}

/**
 * root /var/www/html
 * root <path_to_root>
*/
/**
 * TODO check if path_to_root is valid => if it exists and is a directory ?
 * 		? wip
*/
void	Parser::_parseRoot(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing root directive\n"; // ? debug
	const std::string	rootPath = tokens.at(*i)._getWord();
	if (!isValidPath(rootPath))
		throw PathException(rootPath);
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
 * location <requestUri> { ... }
*/
void Parser::_parseLocationContext(ServerConfig *server, std::vector<Token> tokens, size_t *i) {
	// std::cout << "Parsing location block\n"; // ? debug
	size_t			j = *i;
	const std::string		requestUri = tokens.at(j)._getWord();
	if (requestUri[0] != '/')
		throw UriException(requestUri);
	if (requestUri != "/") {
		if (!isValidPath(requestUri.substr(1)))
			throw UriException(requestUri);
	}
	// if (!isValidUri(requestUri))
	// 	throw InvalidUriException();

	LocationConfig	route(requestUri);

	j += 2; // skip opening brace

	void (Parser::*pf_locationContextDirectives[6])(std::vector<Token> tokens, size_t *j, LocationConfig &route) = {
		&Parser::_parseRoot, 
		&Parser::_parseLocationClientMaxBodySize, 
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
					throw InvalidTokenException("Expected word after: " + tokens.at(j - 1)._getWord());
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
 * TODO make virtual?
*/
void	Parser::_parseServerClientMaxBodySize(ServerConfig *server, std::vector<Token> tokens, size_t *i) {
	// std::cout << "Parsing client_max_body_size directive\n"; // ? debug
	const std::string rawValue = tokens.at(*i)._getWord();
	server->setClientMaxBodySize(parseClientMaxBodySize(rawValue));
}

/**
 * error_page 404 files/html/Website/Error/404.html
 * error_page <error_code (error_code ...)> <path_to_error_page>
*/
/**
 * TODO valid error code ? (400 to 599?)
 * 		? wip
 * TODO valid file path ?
 * 		? wip
*/
void	Parser::_parseErrorPage(ServerConfig *server, std::vector<Token> tokens, size_t *i) {
	// std::cout << "Parsing error_page directive\n"; // ? debug
	size_t						j = *i;
	std::vector<std::string>	errorCodes;
	while (tokens.at(j)._getType() == Token::WORD && isNumber(tokens.at(j)._getWord())) {
		const std::string	errorCode = tokens.at(j)._getWord();
		if (isValidErrorCode(errorCode)) {
			errorCodes.push_back(tokens.at(j)._getWord());
		} else {
			throw ErrorCodeException(errorCode);
		}
		j++;
	}
	const std::string		filePath = tokens.at(j)._getWord();
	if (!isValidPath(filePath))
		throw PathException(filePath);
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
 * 		? wip
*/
void	Parser::_parseServerName(ServerConfig *server, std::vector<Token> tokens, size_t *i) {
	// std::cout << "Parsing server_name directive\n"; // ? debug
	const std::string	serverName = tokens.at(*i)._getWord();
	if (!isValidServerName(serverName)) {
		throw ServerNameException(serverName);
	}
	server->setServerName(serverName);
}

/**
 * listen 8080
 * listen <port_number>
*/
void	Parser::_parseListen(ServerConfig *server, std::vector<Token> tokens, size_t *i) {
	// std::cout << "Parsing listen directive\n"; // ? debug
	const std::string	portNumber = tokens.at(*i)._getWord();
	if (!isValidPortNumber(portNumber))
		throw PortNumberException(portNumber);
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
		&Parser::_parseServerClientMaxBodySize,
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
				throw InvalidTokenException("Expected word after: " + tokens.at(*i - 1)._getWord());
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
		// printCurrentToken(tokens, i); // ? debug
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
	parser._identifyDirectives(&server, server.getTokens());

	// try {
	// 	parser._identifyDirectives(&server, server.getTokens());
	// } catch (const std::exception &e) {
	// 	// TODO set default value or throw error ?
	// 	std::cerr << e.what() << std::endl;
	// 	// return (ServerConfig());
	// }

	// TODO check for N/A directives and set to default values or error ?

	return (server);
}
