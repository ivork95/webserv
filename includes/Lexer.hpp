#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <fstream>
#include <vector>

#include "Token.hpp"

class Lexer {
	public:
		Lexer();
		~Lexer(void);
		
		static std::vector<Token>	tokenizeLine(std::ifstream &configFile);

	private:
		static void					_splitLine(std::vector<Token> *tokens, std::string &line);
};

#endif
