
#include "Parser.hpp"

#include <filesystem>

/**
 * STATIC FUNCTIONS
*/

/**
 * MEMBER FUNCTIONS
*/
void	Parser::_parseServerClientMaxBodySize(ServerConfig *server, std::vector<Token> tokens, size_t *i) {

	const std::string rawValue = tokens.at(*i).getWord();
	server->setClientMaxBodySize(parseClientMaxBodySize(rawValue));
}

void	Parser::_parseErrorPage(ServerConfig *server, std::vector<Token> tokens, size_t *i) {

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
	if (!isValidPath(filePath, false))
		throw PathException(filePath);

	std::filesystem::perms requiredPermissions = OWREAD | OTREAD | GRREAD;
	if (!hasRequiredPermissions(filePath, requiredPermissions))
		throw MissingPermissionsException(filePath + " (r required)");

	ErrorPageConfig	errorPage(errorCodes, filePath);

	server->setErrorPagesConfig(errorPage);
	(*i) = j;
}

// TODO valid server_name??
void	Parser::_parseServerName(ServerConfig *server, std::vector<Token> tokens, size_t *i) {

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
