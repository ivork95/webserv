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
			NEW_LINE,
			NA
		};

		Token(Token::ETokenType &type);
		Token(std::string &word);
		~Token(void);

		static void			printTokens(std::vector<Token> *tokens);

		const Token::ETokenType	&_getType(void) const;
		const std::string		&_getWord(void) const;

		void					setType(const Token::ETokenType &type);
		void					setWord(const std::string &word);

		friend std::ostream		&operator << (std::ostream &out, const Token &token);

	private:
		ETokenType	_type;
		std::string	_word;
	
};

#endif
