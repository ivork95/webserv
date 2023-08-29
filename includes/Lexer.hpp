#ifndef LEXER_HPP
#define LEXER_HPP

#include "Token.hpp"
#include "Configuration.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include <string.h>
#include <string>
#include <algorithm>
#include <vector>
#include <stack>

class Lexer {
	private:
		static void					_splitLine(std::vector<Token> *tokens, std::string &line);

	public:
		Lexer();
		~Lexer(void);
		
		static std::vector<std::string>	splitServers(std::ifstream &configFile);
		static std::vector<Server>		createServers(Configuration *config);
		static std::vector<Token>		tokenizeServer(const std::string &rawData);

		class UnmatchedBracesException : public std::exception {
			public:
				char const* what() const throw() {
					return "Unmatched braces";
				}
		};

};

#endif
