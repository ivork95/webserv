
#include "Lexer.hpp"

/**
 * Constructors / destructors
*/
Lexer::Lexer(void) {
	// std::cout << "Lexer default constructor called\n";
}

Lexer::~Lexer(void) {
	// std::cout << "Lexer destructor called\n";
}

static bool	isSpecialChar(char c) {
	return (c == '{' || c == '}' || c == ';' || c == '\n');
}

/**
 * Member functions
*/
void Lexer::_splitLine(std::vector<Token> *tokens, std::string &line) {
	std::string::iterator it = line.begin();

	// Loop until line end, skipping whitespaces
	while (it != line.end()) {
		while (isspace(*it))
			it++;

		// Get type of current char (word, {, } or ;)
			// If special char
		if (isSpecialChar(*it)) {
			Token::ETokenType type = Token::NA;
			if (*it == ';')
				type = Token::SEMICOLON;
			else if (*it == '{')
				type = Token::OPEN_BRACE;
			else if (*it == '}')
				type = Token::CLOSE_BRACE;
			else if (*it == '\n')
				type = Token::NEW_LINE;
			
			// Add special char to tokens
			Token token(type);
			tokens->push_back(token);
			it++;
		} else {
			std::string word;

			// Loop until reaching another space, special char or end to have a full word
			while (!isspace(*it) && *it != '{' && *it != '}' && *it != ';' && *it != '\n' && it != line.end()) {
				word += *it;
				it++;
			}

			// Add word to tokens
			Token token(word);
			tokens->push_back(token);
		}
	}
}

std::vector<Token> Lexer::tokenizeServer(const std::string &rawData) {
    std::vector<Token> tokens;

    // Split rawData into lines
    std::istringstream linesStream(rawData);
    std::string line;

    while (std::getline(linesStream, line)) {
		// std::cout << line.back() << std::endl; // ? testing
		if (line.empty())
			continue ;
		else if (line.back() != ';' && line.back() != '{' && line.back() != '}')
			return std::vector<Token>();
        // Split line into tokens
        _splitLine(&tokens, line);
    }

    return tokens;
}

std::vector<std::string> Lexer::splitServers(std::ifstream &configFile) {
	std::string 				content((std::istreambuf_iterator<char>(configFile)), \
											(std::istreambuf_iterator<char>()));
    std::vector<std::string>	sections;
    size_t 						startPos = 0;
    std::stack<char> 			braceStack;

    for (size_t i = 0; i < content.size(); ++i) {
        if (content[i] == '{') {
            braceStack.push('{');
            if (braceStack.size() == 1) {
                startPos = i;
            }
        } else if (content[i] == '}') {
            if (!braceStack.empty()) {
                braceStack.pop();
                if (braceStack.empty()) {
                    sections.push_back(content.substr(startPos, i - startPos + 1));
                }
            }
        }
    }
	if (!braceStack.empty()) {
		// throw UnmatchedBracesException();
		std::cerr << "Error: unmatched braces" << std::endl;
		return std::vector<std::string>();
	}
    return sections;
}

std::vector<Server>	Lexer::createServers(Configuration *config) {
	std::vector<Server>	servers;

	for (size_t i = 0; i < config->serverSections.size(); ++i) {
		Server server(i, config->serverSections[i]);
		servers.push_back(server);
	}
	if (servers.empty()) {
		std::cerr << "Error: could not create servers" << std::endl;
		return std::vector<Server>();
	}
	return servers;
}
