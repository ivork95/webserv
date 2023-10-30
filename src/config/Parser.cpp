
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
static int	checkDuplicateRequestUri(std::vector<std::string> &usedRequestUris, const std::string &requestUri) {
	if (std::find(usedRequestUris.begin(), usedRequestUris.end(), requestUri) != usedRequestUris.end())
		return 1;

	usedRequestUris.push_back(requestUri);
	return 0;
}

/**
 * MEMBER FUNCTIONS
*/
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

	return (server);
}
