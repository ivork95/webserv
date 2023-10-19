
#include "Parser.hpp"

#include <filesystem>

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
static std::string	parsePath(const std::string &rawPath, const bool &isDirectory) {

	// Empty
	if (rawPath.empty())
		throw PathException(rawPath);

	// Root path
	if (isDirectory && rawPath == "/")
		return "www";
	
	// Remove leading / 
	std::string inputPath = rawPath;
	// std::cout << "Before leading /: " + inputPath << std::endl; // ? debug
	if (inputPath[0] == '/') {
		inputPath = inputPath.substr(1, inputPath.size());
		// std::cout << "Removed leading /: " + inputPath << std::endl; // ? debug
	}

	// std::cout << "Before checks: " + inputPath << std::endl; // ? debug

	if (isDirectory) {

		// Remove trailing /
		// std::cout << "Before trailing /: " + inputPath << std::endl; // ? debug
		if (inputPath[inputPath.size() - 1] == '/') {
			inputPath = inputPath.substr(0, inputPath.size() - 1);
			// std::cout << "Removed trailing /: " + inputPath << std::endl; // ? debug
		}
		
		if (std::filesystem::is_directory(inputPath)) {
			return inputPath;
		}

		if (std::filesystem::is_directory("www/" + inputPath)) {
			return "www/" + inputPath;
		}

		throw PathException(rawPath);

	} else {
		if (std::filesystem::is_regular_file(inputPath))
			return inputPath;
		throw PathException(rawPath);
	}
}

// TODO MAX_INT >
static int convertToBytes(const std::string &rawValue) {
	std::string convertedValue{};
	for (size_t j = 0; j < rawValue.size() - 1; j++) {
		if (!isdigit(rawValue[j]))
			throw ClientMaxBodySizeException(rawValue);
		else
			convertedValue += rawValue[j];
	}

	if (convertedValue.empty())
		throw ClientMaxBodySizeException(convertedValue);

	const size_t lastCharIndex = rawValue.size() - 1;
	if (rawValue[lastCharIndex] == 'k' || rawValue[lastCharIndex] == 'K')
		convertedValue += "000";
	else if (rawValue[lastCharIndex] == 'm' || rawValue[lastCharIndex] == 'M')
		convertedValue += "000000";
	else if (rawValue[lastCharIndex] == 'g' || rawValue[lastCharIndex] == 'G')
		convertedValue += "000000000";
	
	int nb{};
	std::stringstream as;
	as.str(convertedValue);
	if (!(as >> nb))
		throw ClientMaxBodySizeException(rawValue);

	return nb;
}

static int parseClientMaxBodySize(const std::string &rawValue) {
	if (rawValue == "0")
		return 0;
	else if (!hasConversionUnit(rawValue))
		throw ConversionUnitException(rawValue);
	else
		return (convertToBytes(rawValue));
}

static int	checkDuplicateRequestUri(std::vector<std::string> &usedRequestUris, const std::string &requestUri) {
	if (std::find(usedRequestUris.begin(), usedRequestUris.end(), requestUri) != usedRequestUris.end())
		return 1;

	usedRequestUris.push_back(requestUri);
	return 0;
}

/**
 * MEMBER FUNCTIONS
*/
void	Parser::_parseLimitExcept(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing limit_except directive\n"; // ? debug

	size_t						j = *i;
	std::vector<std::string>	httpMethods;

	while (tokens.at(j).getType() == Token::WORD) {
		if (isValidHttpMethod(tokens.at(j).getWord())) {
			httpMethods.push_back(tokens.at(j).getWord());
			j++;
		} else {
			throw HttpMethodException(tokens.at(j).getWord());
		}
	}

	route.setHttpMethods(httpMethods);
	(*i) = j;
}

/**
 * cgi .php cgi-bin
 * cgi <file_extension> <path_to_cgi>
*/
// TODO cgi?
void	Parser::_parseCgi(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing cgi directive\n"; // ? debug

	if (tokens.at(*i).getWord()[0] == '.') {
		const std::string cgiExtension = tokens.at(*i).getWord();
		if (!isValidCgiExtension(cgiExtension)) // TODO wip
			throw CgiExtensionException(cgiExtension);

		if (tokens.at(*i + 1).getType() == Token::WORD) {
			(*i)++;
			const std::string					cgiPath = tokens.at(*i).getWord();
			if (!isValidPath(cgiPath, true))
				throw PathException(cgiPath);

			std::map<std::string, std::string>	cgiHandler;
			cgiHandler[cgiExtension] = cgiPath;
			route.setCgiHandler(cgiExtension, cgiPath);
		} else {
			throw InvalidTokenException("Missing file path");
		}
	} else {
		throw InvalidTokenException("Expected file extension: " + tokens.at(*i).getWord());
	}
}

/**
 * index index.html index.php
 * index <file_name> (<file_name> ...)
 * ! we are looking for these files in the root directory
*/
void	Parser::_parseIndex(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing index directive\n"; // ? debug

	size_t						j = *i;
	std::vector<std::string>	indexFile;

	while (tokens.at(j).getType() == Token::WORD) {
		if (isValidIndexExtension(tokens.at(j).getWord())) { // TODO wip
			indexFile.push_back(tokens.at(j).getWord());
			j++;
		} else {
			throw IndexException(tokens.at(j).getWord());
		}
	}

	route.setIndexFile(indexFile);
	(*i) = j;
}

void	Parser::_parseAutoIndex(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing autoindex directive\n"; // ? debug

	if (tokens.at(*i).getWord() == "off")
		route.setAutoIndex(false);
	else if (tokens.at(*i).getWord() == "on")
		route.setAutoIndex(true);
	else
		throw AutoIndexException(tokens.at(*i).getWord());
}

void	Parser::_parseLocationClientMaxBodySize(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing client_max_body_size directive\n"; // ? debug
	const std::string rawValue = tokens.at(*i).getWord();
	route.setClientMaxBodySize(parseClientMaxBodySize(rawValue));
}

// TODO check root dir
void	Parser::_parseRoot(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing root directive\n"; // ? debug

	const std::string	rootPath = tokens.at(*i).getWord();
	const std::string	parsedPath = parsePath(rootPath, true);
	// if (rootPath == "/") {
	// 	route.setRootPath(tokens.at(*i).getWord());
	// 	return ;
	// }

	if (!isValidPath(parsedPath, true)) // TODO wip
		throw PathException(rootPath);

	route.setRootPath(parsedPath);
}


// TODO check URI
void Parser::_parseLocationContext(ServerConfig *server, std::vector<Token> tokens, size_t *i) {
	// std::cout << "Parsing location block\n"; // ? debug

	size_t				j = *i;
	const std::string	requestUri = tokens.at(j).getWord();

	if (requestUri[0] != '/')
		throw UriException(requestUri);

	if (!isValidPath(parsePath(requestUri, true), true))
		throw UriException(requestUri);

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

	while (tokens.at(j).getType() != Token::CLOSE_BRACE) {
		for (size_t n = 0; n < 6; n++) {
			if (tokens.at(j).getWord() == locationContextDirectives[n]) {
				j++;
				if (tokens.at(j).getType() == Token::WORD) {
					(this->*pf_locationContextDirectives[n])(tokens, &j, route);
					break;
				} else {
					throw InvalidTokenException("Expected word after: " + tokens.at(j - 1).getWord());
				}
			}
		}
		j++;
	}
	
	// Check for missing directives and set defaults
	route.checkMissingDirective();
	
	server->setLocationsConfig(route);
	(*i) = j;
}

void	Parser::_parseServerClientMaxBodySize(ServerConfig *server, std::vector<Token> tokens, size_t *i) {
	// std::cout << "Parsing client_max_body_size directive\n"; // ? debug

	const std::string rawValue = tokens.at(*i).getWord();
	server->setClientMaxBodySize(parseClientMaxBodySize(rawValue));
}

void	Parser::_parseErrorPage(ServerConfig *server, std::vector<Token> tokens, size_t *i) {
	// std::cout << "Parsing error_page directive\n"; // ? debug

	size_t				j = *i;
	std::vector<int>	errorCodes;

	while (tokens.at(j).getType() == Token::WORD && isNumber(tokens.at(j).getWord())) {
		int	errorCode = std::atoi(tokens.at(j).getWord().c_str());
		if (isValidErrorCode(errorCode)) {
			errorCodes.push_back(errorCode);
		} else {
			throw ErrorCodeException(tokens.at(j).getWord());
		}
		j++;
	}

	if (errorCodes.empty())
		throw ErrorCodeException(tokens.at(j).getWord());

	const std::string			filePath = tokens.at(j).getWord();
	const std::string			parsedPath = parsePath(filePath, false);
	if (!isValidPath(parsedPath, false))
		throw PathException(parsedPath);

	ErrorPageConfig	errorPage(errorCodes, parsedPath);

	server->setErrorPagesConfig(errorPage);
	(*i) = j;
}

// TODO valid server_name??
void	Parser::_parseServerName(ServerConfig *server, std::vector<Token> tokens, size_t *i) {
	// std::cout << "Parsing server_name directive\n"; // ? debug

	size_t						j = *i;
	std::vector<std::string>	serverNames;

	while (tokens.at(j).getType() == Token::WORD) {
		const std::string	serverName = tokens.at(j).getWord();
		
		if (!isValidServerName(serverName)) {
			throw ServerNameException(serverName);
		}

		if (std::find(serverNames.begin(), serverNames.end(), serverName) != serverNames.end()) {
			throw DuplicateServerNameException(serverName);
		}

		serverNames.push_back(serverName);
		j++;
	}
	server->setServerName(serverNames);
}

void	Parser::_parseListen(ServerConfig *server, std::vector<Token> tokens, size_t *i) {
	// std::cout << "Parsing listen directive\n"; // ? debug

	const std::string	portNumber = tokens.at(*i).getWord();
	if (!isValidPortNumber(portNumber))
		throw PortNumberException(portNumber);

	server->setPortNb(portNumber);
}

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

	// Given the current token, loop over the directives and call the appropriate function
	for (size_t n = 0; n < 5; n++) {
		if (tokens.at(*i).getWord() == serverContextDirectives[n]) {
			(*i)++;
			if (tokens.at(*i).getType() == Token::WORD) {
				(this->*pf_serverContextDirectives[n])(server, tokens, i);
				break;
			} else {
				throw InvalidTokenException("Expected word after: " + tokens.at(*i - 1).getWord());
			}
		}
	}
}

ServerConfig Parser::parseTokens(ServerConfig server) {
	Parser parser;

	for (size_t i = 0; i < server.getTokens().size(); i++) {
		// std::cout << "[" << i << "] " << server.getTokens().at(i) << std::endl; // ? debug
		parser._parseServerContext(&server, server.getTokens(), &i);
	}

	// Check for missing directives and set defaults
	server.checkMissingDirective();

	// Check for duplicate request URIs
	std::vector<std::string>	usedRequestUris;
	for (size_t i = 0; i < server.getLocationsConfig().size(); i++) {
		const std::string requestUri = server.getLocationsConfig()[i].getRequestURI();
		if (checkDuplicateRequestUri(usedRequestUris, requestUri)) {
			throw DuplicateRequestUriException(requestUri);
		}
	}

	// TODO check root, URI, ...

	return (server);
}
