
#include "Lexer.hpp"

Token::Token(void) {
	std::cout << "Token constructor called\n";
}

Token::~Token(void) {
	std::cout << "Token destructor called\n";
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
		std::cout << line << std::endl;
	}

	return tokens;
}
