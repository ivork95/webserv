
#include "Token.hpp"

Token::Token(Token::ETokenType &type) : _type(type), _word("") {
	// std::cout << "Token constructor called\n";
}

Token::Token(std::string &word) : _type(Token::NA), _word(word) {
	// std::cout << "Token constructor called\n";
}

Token::~Token(void) {
	// std::cout << "Token destructor called\n";
}

Token::ETokenType Token::_getType(void) const {
	return _type;
}

std::string Token::_getWord(void) const {
	return _word;
}

void	Token::printTokens(std::vector<Token> *tokens) {
	std::cout << "Tokens:\n";
	for (std::vector<Token>::iterator it = tokens->begin(); it != tokens->end(); it++) {
		if (it->_getType() != Token::NA)
			std::cout << "\ttype: " << it->_getType() << std::endl;
		else
			std::cout << "\tword: " << it->_getWord() << std::endl;
	}
}
