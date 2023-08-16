#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <fstream>
#include <vector>

class Token {
	public:
		enum ETokenType {
			WORD,
			SEMICOLON,
			OPEN_BRACE,
			CLOSE_BRACE,
			NA
		};

		Token(Token::ETokenType &type);
		Token(std::string &word);
		~Token(void);


	private:
		ETokenType	_type;
		std::string	_word;

		Token::ETokenType	_getType(void) const;
		std::string			_getWord(void) const;
};

class Lexer {
	public:
		Lexer();
		~Lexer(void);
		
		static std::vector<Token>	tokenizeLine(std::ifstream &configFile);

	private:
		static void					_splitLine(std::vector<Token> *tokens, std::string &line);
};

#endif
