#ifndef PARSER_HPP
#define PARSER_HPP

#include "Lexer.hpp"

#include <iostream>

class Parser {
	public:
		Parser();
		~Parser(void);

		void	parseTokens(std::vector<Token> *tokens);

	private:
		// parse tokens into data structures => Config.hpp
};

#endif
