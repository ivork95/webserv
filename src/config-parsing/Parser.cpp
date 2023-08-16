
#include "Parser.hpp"

Parser::Parser(void) {
	// std::cout << "Parser constructor called\n";
}

Parser::~Parser(void) {
	// std::cout << "Parser destructor called\n";
}

void	Parser::parseTokens(std::vector<Token> *tokens) {
	Token::printTokens(tokens); // ? testing

	// identify sections and directives

	// process directives (port, server name, error page, ...)
		// + validate directives

	// build config objects (data structures)
}
