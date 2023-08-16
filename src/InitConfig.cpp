
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Configuration.hpp"

#include <iostream>
#include <fstream>
#include <string.h>

static int	parseTokens(std::vector<Token> *tokens) {
	Parser parser;

	parser.parseTokens(tokens);
	return (0);
}

static int	readFile(std::ifstream &configFile, std::vector<Token> *tokens) {
	*tokens = Lexer::tokenizeLine(configFile);
	if (tokens->empty()) {
		std::cerr << "Error: could not tokenize file" << std::endl;
		return (1);
	}
	return (0);
}

static int	openFile(std::ifstream &configFile, const std::string &filePath) {
	configFile.open(filePath);
	if (!configFile.is_open()) {
		std::cerr << "Error: could not open file " << filePath << std::endl;
		return (1);
	}
	return (0);
}

int initConfig(const std::string &filePath) {
	std::ifstream configFile;
	std::vector<Token> tokens;

	// open file for read
	if (openFile(configFile, filePath))
		return (1);

	// read & tokenize line by line
	if (readFile(configFile, &tokens))
		return (1);

	// Token::printTokens(&tokens); // ? testing

	// parse tokens
	if (parseTokens(&tokens))
		return (1);

	// close file
	configFile.close();

	return (0);
}