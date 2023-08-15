#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <fstream>
#include <vector>

class Token {
	public:
		// Token(void) = delete;
		Token();
		~Token(void);

	private:
		// token types
		// token value
};

class Lexer {
	public:
		// Lexer(void) = delete;
		Lexer();
		~Lexer(void);
		
		// tokenize the config file line by line
		static std::vector<Token> tokenizeLine(std::ifstream &configFile);

	private:
};

#endif
