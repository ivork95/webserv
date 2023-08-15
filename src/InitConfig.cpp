
#include "config-parsing/Lexer.hpp"
#include "config-parsing/Parser.hpp"
#include "config-parsing/Configuration.hpp"

#include <iostream>
#include <fstream>
#include <string.h>

static int	readFile(std::ifstream &configFile) {
	std::vector<Token> tokens = Lexer::tokenizeLine(configFile);
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

	// open file for read
	if (openFile(configFile, filePath))
		return (1);

	// read & tokenize line by line
	if (readFile(configFile))
		return (1);

	// parse tokens
		// identify sections and directives

		// process directives (port, server name, error page, ...)
			// + validate directives

		// build config objects (data structures)

	// close file
	configFile.close();

	return (0);
}