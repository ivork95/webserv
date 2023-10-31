
#include "Parser.hpp"

#include <filesystem>

/**
 * STATIC FUNCTIONS
*/
static std::string removeExtraSlashes(const std::string &path) {
    size_t start = path.find_first_not_of('/');
    size_t end = path.find_last_not_of('/');

    if (start == std::string::npos || end == std::string::npos) {
        return "";
	}

	std::string cleanedPath = path.substr(start, end - start + 1);
    return cleanedPath;
}

static int checkCgiScriptAbsPath(LocationConfig &route) {
	const std::string scriptFile = removeExtraSlashes(route.getCgiScript());
    const std::string requestUri = removeExtraSlashes(route.getRequestURI());
    const std::string rootPath = removeExtraSlashes(route.getRootPath());

	std::string scriptAbsolutePath{};
	if (!rootPath.empty()) {
		scriptAbsolutePath = rootPath;
	}
	if (!requestUri.empty()) {
		scriptAbsolutePath = scriptAbsolutePath.append("/" + requestUri);
	}
	if (!scriptFile.empty()) {
		scriptAbsolutePath = scriptAbsolutePath.append("/" + scriptFile);
	}

	std::filesystem::path path(scriptAbsolutePath);
    if (!std::filesystem::exists(path))
		return 1;
	
	std::filesystem::perms requiredPermissions = OWREAD | OWEXEC; // ? add permissions here
	if (!hasRequiredPermissions(path, requiredPermissions))
		return 2;

	route.setAbsCgiScript(path);

    return 0;
}

/**
 * MEMBER FUNCTIONS
*/
void	Parser::_parseLimitExcept(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing limit_except directive\n"; // ? debug

	size_t						j = *i;
	std::vector<std::string>	httpMethods;

	while (tokens.at(j).getType() == Token::WORD) {
		if (isValidHttpMethod(tokens.at(j).getWord())) {
			httpMethods.push_back(tokens.at(j).getWord());
			j++;
		} else {
			throw HttpMethodException(tokens.at(j).getWord());
		}
	}

	route.setHttpMethods(httpMethods);
	(*i) = j;
}

void	Parser::_parseCgi(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing cgi directive\n"; // ? debug

	const std::string cgiScript = tokens.at(*i).getWord();
	if (cgiScript.empty())
		throw InvalidTokenException("Missing CGI script: " + cgiScript);
	
	if (!isValidCgiScript(cgiScript))
		throw CgiException("Invalid script: " + cgiScript);

	route.setCgiScript(cgiScript);

	if (tokens.at(*i + 1).getType() == Token::WORD) {
		(*i)++;

		const std::string cgiInterpreter = tokens.at(*i).getWord();
		if (cgiInterpreter.empty())
			throw InvalidTokenException("Missing CGI interpreter after: " + cgiScript);
	
		if (!isValidCgiInterpreter(cgiInterpreter))
			throw CgiException("Invalid interpreter: " + cgiInterpreter);

		std::filesystem::perms requiredPermissions = OWREAD | OWWRITE | OWEXEC | OTREAD | OTEXEC | GRREAD | GREXEC;
		if (!hasRequiredPermissions(cgiInterpreter, requiredPermissions))
			throw CgiException("Invalid permissions for interpreter: " + cgiInterpreter + " (r-w-x required)");

		route.setCgiInterpreter(cgiInterpreter);

	} else {
		throw InvalidTokenException("Missing CGI interpreter after: " + cgiScript);
	}
}

void	Parser::_parseIndex(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing index directive\n"; // ? debug

	size_t						j = *i;
	std::vector<std::string>	indexFile;

	while (tokens.at(j).getType() == Token::WORD) {
		if (isValidIndexExtension(tokens.at(j).getWord())) {
			indexFile.push_back(tokens.at(j).getWord());
			j++;
		} else {
			throw IndexException(tokens.at(j).getWord());
		}
	}

	route.setIndexFile(indexFile);
	(*i) = j;
}

void	Parser::_parseAutoIndex(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing autoindex directive\n"; // ? debug

	if (tokens.at(*i).getWord() == "off")
		route.setAutoIndex(false);
	else if (tokens.at(*i).getWord() == "on")
		route.setAutoIndex(true);
	else
		throw AutoIndexException(tokens.at(*i).getWord());
}

void	Parser::_parseLocationClientMaxBodySize(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing client_max_body_size directive\n"; // ? debug
	const std::string rawValue = tokens.at(*i).getWord();
	route.setClientMaxBodySize(parseClientMaxBodySize(rawValue));
}

void	Parser::_parseRoot(std::vector<Token> tokens, size_t *i, LocationConfig &route) {
	// std::cout << "\tParsing root directive\n"; // ? debug

	const std::string	rootPath = tokens.at(*i).getWord();

	if (!isValidPath(rootPath, true))
		throw PathException(rootPath);

	route.setRootPath(rootPath);
}

void Parser::_parseLocationContext(ServerConfig *server, std::vector<Token> tokens, size_t *i) {
	// std::cout << "Parsing location block\n"; // ? debug

	size_t				j = *i;
	const std::string	requestUri = tokens.at(j).getWord();

	if (!isValidUri(requestUri))
		throw UriException(requestUri);

	LocationConfig	route(requestUri);

	j += 2; // skip opening brace

	void (Parser::*pf_locationContextDirectives[6])(std::vector<Token> tokens, size_t *j, LocationConfig &route) = {
		&Parser::_parseRoot, 
		&Parser::_parseLocationClientMaxBodySize, 
		&Parser::_parseAutoIndex, 
		&Parser::_parseIndex, 
		&Parser::_parseCgi,
		&Parser::_parseLimitExcept
	};
	std::string locationContextDirectives[6] = {
		"root", 
		"client_max_body_size", 
		"autoindex", 
		"index", 
		"cgi",
		"limit_except"
	};

	while (tokens.at(j).getType() != Token::CLOSE_BRACE) {
		for (size_t n = 0; n < 6; n++) {
			if (tokens.at(j).getWord() == locationContextDirectives[n]) {
				j++;
				if (tokens.at(j).getType() == Token::WORD) {
					(this->*pf_locationContextDirectives[n])(tokens, &j, route);
					break;
				} else {
					throw InvalidTokenException("Expected word after: " + tokens.at(j - 1).getWord());
				}
			}
		}
		j++;
	}
	
	route.checkMissingDirective();

	if (route.hasCgiScript()) {
		const int res = checkCgiScriptAbsPath(route);
		switch (res)
		{
		case 1:
			throw CgiException("File does not exist: " + route.getRootPath() + route.getRequestURI() + route.getCgiScript());
			break;
		case 2:
			throw CgiException("Invalid permissions: " + route.getCgiScript() + " (r-x required)");
			break;
		default:
			// std::cout << route.getAbsCgiScript() << std::endl; // ? debug
			break;
		}
	}

	server->setLocationsConfig(route);
	(*i) = j;
}
