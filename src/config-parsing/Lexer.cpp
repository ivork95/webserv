
#include "Lexer.hpp"

Lexer::Lexer(void) {
	// std::cout << "Lexer constructor called\n";
}

Lexer::~Lexer(void) {
	// std::cout << "Lexer destructor called\n";
}

static bool	isSpecialChar(char c) {
	return (c == '{' || c == '}' || c == ';');
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
		// std::cout << "full line: " << line << std::endl; // ? testing

		// Split line into tokens
		_splitLine(&tokens, line);
	}
	Token::printTokens(&tokens);

	return tokens;
}

void Lexer::_splitLine(std::vector<Token> *tokens, std::string &line) {
	std::string::iterator it = line.begin();

	// Loop until line end, skipping whitespaces
	while (it != line.end()) {
		while (isspace(*it))
			it++;

		// Get type of current char (word, {, } or ;)
			// If special char
		if (isSpecialChar(*it)) {
			Token::ETokenType type = Token::NA;
			if (*it == '{')
				type = Token::OPEN_BRACE;
			else if (*it == '}')
				type = Token::CLOSE_BRACE;
			else if (*it == ';')
				type = Token::SEMICOLON;

			// Add special char to tokens
			Token token(type);
			tokens->push_back(token);
			it++;
		}
		else {
			std::string word;

			// Loop until reaching another space, special char or end to have a full word
			while (!isspace(*it) && *it != '{' && *it != '}' && *it != ';') {
				word += *it;
				it++;
			}

			// Add word to tokens
			Token token(word);
			tokens->push_back(token);
		}
	}
}
