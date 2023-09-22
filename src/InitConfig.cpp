
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Configuration.hpp"

static int	checkDuplicatePortNumbers(std::vector<std::string> &usedPorts, const std::string &portNb) {
	if (std::find(usedPorts.begin(), usedPorts.end(), portNb) != usedPorts.end()) {
		return 1;
	}
	usedPorts.push_back(portNb);
	return 0;
}

static int	parseTokens(Configuration *config) {
    std::vector<std::string>	usedPorts;
	for (size_t i = 0; i < config->serversConfig.size(); ++i) {
		try {
			config->serversConfig[i] = Parser::parseTokens(config->serversConfig[i]);
		} catch (const std::exception &e) {
			std::cerr << "Error: Parsing server " << i << std::endl;
			std::cerr << e.what() << std::endl;
			return (1);
		}
		const std::string portNb = config->serversConfig[i].getPortNb();
		if (checkDuplicatePortNumbers(usedPorts, portNb)) {
			std::cerr << "Error: Duplicate port number: " << portNb << std::endl;
			return (1);
		}
	}
	return (0);
}

static int	tokenizeServers(Configuration *config) {
	for (size_t i = 0; i < config->serversConfig.size(); ++i) {
		try {
			config->serversConfig[i].setTokens(Lexer::tokenizeServer(config->serversConfig[i].getRawData()));
		} catch (const std::exception &e) {
			std::cerr << "Error: Tokenizing server " << i << std::endl;
			std::cerr << e.what() << std::endl;
			return (1);
		}
	}
	return (0);
}

static int	createServers(Configuration *config) {
	try {
		config->serversConfig = Lexer::createServers(config);
	} catch (const std::exception &e) {
		std::cerr << "Error: Creating server config" << std::endl;
		std::cerr << e.what() << std::endl;
		return (1);
	}
	return (0);
}

static int	readFile(std::ifstream &configFile, Configuration *config) {
	try {
		config->serverSections = Lexer::splitServers(configFile);
	} catch (const std::exception &e) {
		std::cerr << "Error: Splitting server blocks" << std::endl;
		std::cerr << e.what() << std::endl;
		return (1);
	}
	return (0);
}

static int	openFile(std::ifstream &configFile, const std::string &filePath) {
	configFile.open(filePath);
	if (!configFile.is_open()) {
		std::cerr << "Error: Opening file " << filePath << std::endl;
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
	std::cout << "\n\t\t -----------------\n";
	std::cout << "\t\t[  SERVER CONFIG  ]\n";
	std::cout << "\t\t -----------------\n";
	for (size_t i = 0; i < config.serversConfig.size(); ++i) {
		std::cout << config.serversConfig[i];
	}

	return (0);
}
