
#include "Lexer.hpp"

Token::Token(Token::ETokenType &type) : _type(type), _word("") {
	std::cout << "Token constructor called\n";
}

Token::Token(std::string &word) : _type(Token::NA), _word(word) {
	std::cout << "Token constructor called\n";
}

Token::~Token(void) {
	std::cout << "Token destructor called\n";
}

Token::ETokenType Token::getType(void) const {
	return _type;
}

std::string Token::getWord(void) const {
	return _word;
}

Lexer::Lexer(void) {
	std::cout << "Lexer constructor called\n";
}

Lexer::~Lexer(void) {
	std::cout << "Lexer destructor called\n";
}

std::vector<Token> Lexer::tokenizeLine(std::ifstream &configFile) {
	std::vector<Token> tokens;

	// Check if file is opened
	if (!configFile.is_open()) {
		std::cout << "Error: file not opened\n";
		return tokens;
	}

	// Read line by line (getline)
	std::string line;
	while (std::getline(configFile, line)) {
		std::cout << "full line: " << line << std::endl;

		// Split line into tokens
		_splitLine(&tokens, line);
	}

	return tokens;
}

void Lexer::_splitLine(std::vector<Token> *tokens, std::string &line) {
	std::string::iterator it = line.begin();

	// Loop until line end, skipping whitespaces
	while (it != line.end()) {
		while (isspace(*it))
			it++;

		std::cout << *it << std::endl;
		// Get type of current char (word, {, } or ;)
			// If word
				// Loop until reaching another space, special char or end to have a full word
			// Else
				// Add special char to tokens

		it++;
	}
	(void)tokens;
}