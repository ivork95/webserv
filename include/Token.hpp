#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <iostream>
#include <vector>

#include "Logger.hpp"

class Token
{
public:
    enum ETokenType
    {
        WORD,
        SEMICOLON,
        OPEN_BRACE,
        CLOSE_BRACE,
        NEW_LINE,
        NA
    };
    Token(void) = delete;

    // constructor
    Token(Token::ETokenType &type);

    // constructor
    Token(std::string &word);

    // getters/setters
    const Token::ETokenType &getType(void) const;
    const std::string &getWord(void) const;
    void setType(const Token::ETokenType &type);
    void setWord(const std::string &word);

    friend std::ostream &operator<<(std::ostream &out, const Token &token);

private:
    ETokenType _type;
    std::string _word;
};

#endif