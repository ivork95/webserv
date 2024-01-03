#ifndef PARSER_HPP
#define PARSER_HPP

#include "Lexer.hpp"

#include "ParserHelper.hpp"

#define SERVER_FUNC_N 5
#define LOC_FUNC_N 6

class Parser;
class ServerConfig;

typedef void (Parser::*ParserFunc)(void *dest, std::vector<Token> tokens, size_t *i);

typedef struct s_parseFuncKey
{
	std::string key;
	ParserFunc func;
} t_parseFuncKey;

class Parser
{
private:
	void _parseServerContext(ServerConfig *server, std::vector<Token> tokens, size_t *i);

	void _parseListen(void *dest, std::vector<Token> tokens, size_t *i);
	void _parseServerName(void *dest, std::vector<Token> tokens, size_t *i);
	void _parseErrorPage(void *dest, std::vector<Token> tokens, size_t *i);
	void _parseServerClientMaxBodySize(void *dest, std::vector<Token> tokens, size_t *i);
	void _parseLocationContext(void *dest, std::vector<Token> tokens, size_t *i);

	void _parseRoot(void *dest, std::vector<Token> tokens, size_t *i);
	void _parseLocationClientMaxBodySize(void *dest, std::vector<Token> tokens, size_t *i);
	void _parseIndex(void *dest, std::vector<Token> tokens, size_t *i);
	void _parseAutoIndex(void *dest, std::vector<Token> tokens, size_t *i);
	void _parseCgi(void *dest, std::vector<Token> tokens, size_t *i);
	void _parseLimitExcept(void *dest, std::vector<Token> tokens, size_t *i);

public:
	// default constructor
	Parser(void);

	static ServerConfig parseTokens(ServerConfig server);
	static t_parseFuncKey serverContextFuncs[SERVER_FUNC_N];
	static t_parseFuncKey locationContextFuncs[LOC_FUNC_N];
};

#endif
