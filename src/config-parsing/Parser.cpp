
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
	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (tokens.at(i)._getType() == Token::NA)
		{
			if (tokens.at(i)._getWord() == "listen")
			{
				if (tokens.at(i + 1)._getType() == Token::NA)
				{
					server->port = tokens.at(i + 1)._getWord();
					i++;
				}
				else
				{
					// throw error
				}
			}
			else if (tokens.at(i)._getWord() == "server_name")
			{
				if (tokens.at(i + 1)._getType() == Token::NA)
				{
					server->serverName = tokens.at(i + 1)._getWord();
					i++;
				}
				else
				{
					// throw error
				}
			}
			else if (tokens.at(i)._getWord() == "error_page")
			{
				if (tokens.at(i + 1)._getType() == Token::NA)
				{
					server->errorPages.push_back(tokens.at(i + 1)._getWord());
					i++;
				}
				else
				{
					// throw error
				}
			}
			else if (tokens.at(i)._getWord() == "client_body_size_limit")
			{
				if (tokens.at(i + 1)._getType() == Token::NA)
				{
					server->clientBodySizeLimit = atoi(tokens.at(i + 1)._getWord().c_str());
					i++;
				}
				else
				{
					// throw error
				}
			}
			else if (tokens.at(i)._getWord() == "location")
			{
				if (tokens.at(i + 1)._getType() == Token::NA)
				{
					Route route(tokens.at(i + 1)._getWord());
					server->routes.push_back(route);
					i++;
				}
				else
				{
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
