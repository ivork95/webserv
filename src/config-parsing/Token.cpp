
#include "Token.hpp"

/**
 * CONSTRUCTORS / DESTRUCTORS
*/
Token::Token(Token::ETokenType &type) : _type(type), _word("") {
	// std::cout << "Token type constructor called\n";
}

Token::Token(std::string &word) : _type(Token::WORD), _word(word) {
	// std::cout << "Token word constructor called\n";
}

Token::~Token(void) {
	// std::cout << "Token destructor called\n";
}

/**
 * Getters / setters
*/
const Token::ETokenType &Token::_getType(void) const {
	return (_type);
}

const std::string &Token::_getWord(void) const {
	return (_word);
}

void	Token::setType(const Token::ETokenType &type) {
	_type = type;
}

void	Token::setWord(const std::string &word) {
	_word = word;
}

/**
 * OPERATOR OVERLOADS
*/
std::ostream &operator << (std::ostream &out, const Token &token) {
	if (token._getType() != Token::WORD)
		out << "type: " << token._getType();
	else
		out << "word: " << token._getWord();
	return (out);
}

/**
 * MEMBER FUNCTIONS
*/
void	Token::printTokens(std::vector<Token> *tokens) {
	std::cout << "Tokens:\n";
	for (std::vector<Token>::iterator it = tokens->begin(); it != tokens->end(); it++) {
		std::cout << *it << std::endl;
	}
}
