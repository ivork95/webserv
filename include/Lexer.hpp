#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <string>
#include <algorithm>
#include <vector>
#include <stack>

#include "Token.hpp"
#include "Configuration.hpp"

class Lexer
{
private:
    static void _splitLine(std::vector<Token> *tokens, std::string &line);

public:
    Lexer(void);
    ~Lexer(void);

    static std::vector<std::string> splitServers(std::ifstream &configFile);
    static std::vector<ServerConfig> createServers(Configuration *config);
    static std::vector<Token> tokenizeServer(const std::string &rawData);

    class InvalidConfigFileException : public std::exception
    {
    public:
        InvalidConfigFileException(const std::string &derivedMsg)
            : derivedErrorMessage(derivedMsg)
        {
            fullErrorMessage = "Invalid config file: " + derivedErrorMessage;
        }

        const char *what() const noexcept override
        {
            return fullErrorMessage.c_str();
        }

    private:
        std::string derivedErrorMessage;
        std::string fullErrorMessage;
    };

    class UnmatchedBracesException : public InvalidConfigFileException
    {
    public:
        UnmatchedBracesException(void)
            : InvalidConfigFileException("Unmatched braces") {}
    };

    class EmptyFileException : public InvalidConfigFileException
    {
    public:
        EmptyFileException(void)
            : InvalidConfigFileException("Empty file") {}
    };

    class NoServerSectionException : public InvalidConfigFileException
    {
    public:
        NoServerSectionException(void)
            : InvalidConfigFileException("No server section found") {}
    };

    class NoServerBlockException : public InvalidConfigFileException
    {
    public:
        NoServerBlockException(void)
            : InvalidConfigFileException("No server block found") {}
    };
};

#endif
