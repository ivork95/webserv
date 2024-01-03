#ifndef LEXER_HPP
#define LEXER_HPP

#include "Configuration.hpp"
#include "Token.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include <string.h>
#include <string>

#include <algorithm>
#include <vector>
#include <stack>

class Configuration;

class Lexer
{
private:
    static void _splitLine(std::vector<Token> *tokens, std::string &line);

public:
    // default constructor
    Lexer(void);

    static std::vector<std::string> splitServers(std::ifstream &configFile);
    static std::vector<ServerConfig> createServers(Configuration *config);
    static std::vector<Token> tokenizeServer(const std::string &rawData);
};

#endif
