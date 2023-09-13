
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Configuration.hpp"

static int	parseTokens(Configuration *config) {
	for (size_t i = 0; i < config->serversConfig.size(); ++i) {
		try {
			config->serversConfig[i] = Parser::parseTokens(config->serversConfig[i]);
		} catch (const std::exception &e) {
			std::cerr << "Error: could not parse server " << i << std::endl;
			std::cerr << e.what() << std::endl;
			return (1);
		}
	}
	if (config->serversConfig.empty()) {
		std::cerr << "Error: could not parse tokens" << std::endl;
		return (1);
	}
	return (0);
}

static int	tokenizeServers(Configuration *config) {
	for (size_t i = 0; i < config->serversConfig.size(); ++i) {
		// config->serversConfig[i].tokens = Lexer::tokenizeServer(config->serversConfig[i].rawData);
		config->serversConfig[i].setTokens(Lexer::tokenizeServer(config->serversConfig[i].getRawData()));
		if (config->serversConfig[i].getTokens().empty()) {
			std::cerr << "Error: could not tokenize server " << i << std::endl;
			return (1);
		}
	}
	return (0);
}

static int	createServers(Configuration *config) {
	config->serversConfig = Lexer::createServers(config);
	if (config->serversConfig.empty()) {
		std::cerr << "Error: could not create serversConfig" << std::endl;
		return (1);
	}
	return (0);
}

static int	readFile(std::ifstream &configFile, Configuration *config) {
	try {
		config->serverSections = Lexer::splitServers(configFile);
	} catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
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
	std::ifstream 		configFile;
	Configuration 		config;

	// open file for read
	if (openFile(configFile, filePath))
		return (1);

	// read file, check braces & split sections
	if (readFile(configFile, &config))
		return (1);
	
	// close file
	configFile.close();

	// create Server objects from server sections
	if (createServers(&config))
		return (1);

	// tokenize each server
	if (tokenizeServers(&config))
		return (1);

	// parse tokens into data structures
	if (parseTokens(&config))
		return (1);

	// ? debug
	std::cout << "\n\n\t\t[SERVERS CONFIG ]\n\n";
	for (size_t i = 0; i < config.serversConfig.size(); ++i) {
		std::cout << config.serversConfig[i];
	}

	return (0);
}
