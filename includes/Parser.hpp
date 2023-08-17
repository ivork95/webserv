#ifndef PARSER_HPP
#define PARSER_HPP

#include "Lexer.hpp"
#include "Configuration.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include <string.h>
#include <string>
#include <algorithm>
#include <vector>
#include <stack>

class Parser {
	public:
		Parser();
		~Parser(void);

		static Server	parseTokens(Server server);

	private:
		static void _identifyDirectives(Server *server, std::vector<Token> tokens);

};

#endif
