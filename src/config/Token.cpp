
#include "Token.hpp"

/**
 * CONSTRUCTORS / DESTRUCTORS
*/
Token::Token(Token::ETokenType &type) : _type(type), _word("") {
	// Logger::getInstance().debug("Token type constructor called");
}

Token::Token(std::string &word) : _type(Token::WORD), _word(word) {
	// Logger::getInstance().debug("Token word constructor called");
}

Token::~Token(void) {
	// std::cout << "Token destructor called\n";
}

/**
 * GETTERS / SETTERS
*/
const Token::ETokenType &Token::getType(void) const {
	return (_type);
}

const std::string &Token::getWord(void) const {
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
	if (token.getType() != Token::WORD)
		out << token.getType();
	else
		out << token.getWord();
	return (out);
}

/**
 * MEMBER FUNCTIONS
*/
