#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "Logger.hpp"

#include <iostream>
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

		static void			printTokens(std::vector<Token> *tokens);

	private:
		ETokenType	_type;
		std::string	_word;

		Token::ETokenType	_getType(void) const;
		std::string			_getWord(void) const;
};

#endif
