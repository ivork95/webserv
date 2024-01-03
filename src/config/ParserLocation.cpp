
#include "Parser.hpp"

/**
 * STATIC FUNCTIONS
 */
t_parseFuncKey Parser::locationContextFuncs[LOC_FUNC_N] = {
	{"root", &Parser::_parseRoot},
	{"client_max_body_size", &Parser::_parseLocationClientMaxBodySize},
	{"autoindex", &Parser::_parseAutoIndex},
	{"index", &Parser::_parseIndex},
	{"cgi", &Parser::_parseCgi},
	{"limit_except", &Parser::_parseLimitExcept},
};

static int checkCgiScriptAbsPath(LocationConfig &route)
{
	const std::string scriptFile = ParserHelper::removeExtraSlashes(route.getCgiScript());
	const std::string requestUri = ParserHelper::removeExtraSlashes(route.getRequestURI());
	const std::string rootPath = ParserHelper::removeExtraSlashes(route.getRootPath());

	std::string scriptAbsolutePath{};
	if (!rootPath.empty())
	{
		scriptAbsolutePath = rootPath;
	}
	if (!requestUri.empty())
	{
		scriptAbsolutePath = scriptAbsolutePath.append("/" + requestUri);
	}
	if (!scriptFile.empty())
	{
		scriptAbsolutePath = scriptAbsolutePath.append("/" + scriptFile);
	}

	std::filesystem::path path(scriptAbsolutePath);
	if (!std::filesystem::exists(path))
		return 1;

	std::filesystem::perms requiredPermissions = OWREAD | OWEXEC | OWWRITE | OTREAD | OTEXEC | GRREAD | GREXEC; // ? add permissions here
	if (!ParserHelper::hasRequiredPermissions(path, requiredPermissions))
		return 2;

	route.setAbsCgiScript(path);

	return 0;
}

static void checkCgiScript(LocationConfig &route)
{
	const int res = checkCgiScriptAbsPath(route);
	switch (res)
	{
	case 1:
		throw CgiException("File does not exist: " + route.getRootPath() + route.getRequestURI() + "/" + route.getCgiScript());
		break;
	case 2:
		throw CgiException("Invalid permissions: " + route.getCgiScript() + " (r-x required)");
		break;
	default:
		break;
	}
}

/**
 * MEMBER FUNCTIONS
 */
void Parser::_parseLimitExcept(void *dest, std::vector<Token> tokens, size_t *i)
{
	LocationConfig *route = static_cast<LocationConfig *>(dest);

	size_t j = *i;
	std::vector<std::string> httpMethods;

	while (tokens.at(j).getType() == Token::WORD)
	{
		if (ParserHelper::isValidHttpMethod(tokens.at(j).getWord()))
		{
			httpMethods.push_back(tokens.at(j).getWord());
			j++;
		}
		else
		{
			throw HttpMethodException(tokens.at(j).getWord());
		}
	}

	route->setHttpMethods(httpMethods);
	(*i) = j;
}

void Parser::_parseCgi(void *dest, std::vector<Token> tokens, size_t *i)
{
	LocationConfig *route = static_cast<LocationConfig *>(dest);

	const std::string cgiScript = tokens.at(*i).getWord();
	if (cgiScript.empty())
		throw InvalidTokenException("Missing CGI script: " + cgiScript);

	if (!ParserHelper::isValidCgiScript(cgiScript))
		throw CgiException("Invalid script: " + cgiScript);

	route->setCgiScript(cgiScript);

	if (tokens.at(*i + 1).getType() == Token::WORD)
	{
		(*i)++;

		const std::string cgiInterpreter = tokens.at(*i).getWord();
		if (cgiInterpreter.empty())
			throw InvalidTokenException("Missing CGI interpreter after: " + cgiScript);

		if (!ParserHelper::isValidCgiInterpreter(cgiInterpreter))
			throw CgiException("Invalid interpreter: " + cgiInterpreter);

		std::filesystem::perms requiredPermissions = OWREAD | OWWRITE | OWEXEC | OTREAD | OTEXEC | GRREAD | GREXEC;
		if (!ParserHelper::hasRequiredPermissions(cgiInterpreter, requiredPermissions))
			throw CgiException("Invalid permissions for interpreter: " + cgiInterpreter + " (r-w-x required)");

		route->setCgiInterpreter(cgiInterpreter);
	}
	else
	{
		throw InvalidTokenException("Missing CGI interpreter after: " + cgiScript);
	}
}

void Parser::_parseIndex(void *dest, std::vector<Token> tokens, size_t *i)
{
	LocationConfig *route = static_cast<LocationConfig *>(dest);

	size_t j = *i;
	std::vector<std::string> indexFile;

	while (tokens.at(j).getType() == Token::WORD)
	{
		if (ParserHelper::isValidIndexExtension(tokens.at(j).getWord()))
		{
			indexFile.push_back(tokens.at(j).getWord());
			j++;
		}
		else
		{
			throw IndexException(tokens.at(j).getWord());
		}
	}

	route->setIndexFile(indexFile);
	(*i) = j;
}

void Parser::_parseAutoIndex(void *dest, std::vector<Token> tokens, size_t *i)
{
	LocationConfig *route = static_cast<LocationConfig *>(dest);

	if (tokens.at(*i).getWord() == "off")
	{
		route->setAutoIndex(false);
	}
	else if (tokens.at(*i).getWord() == "on")
	{
		route->setAutoIndex(true);
	}
	else
		throw AutoIndexException(tokens.at(*i).getWord());
}

void Parser::_parseLocationClientMaxBodySize(void *dest, std::vector<Token> tokens, size_t *i)
{
	LocationConfig *route = static_cast<LocationConfig *>(dest);

	const std::string rawValue = tokens.at(*i).getWord();
	route->setClientMaxBodySize(ParserHelper::parseClientMaxBodySize(rawValue));
}

void Parser::_parseRoot(void *dest, std::vector<Token> tokens, size_t *i)
{
	LocationConfig *route = static_cast<LocationConfig *>(dest);

	const std::string rootPath = tokens.at(*i).getWord();

	if (rootPath.back() != '/')
		throw PathException(rootPath + " (must end with '/)");

	if (!ParserHelper::isValidPath(rootPath, true))
		throw PathException(rootPath);

	route->setRootPath(rootPath);
}

void Parser::_parseLocationContext(void *dest, std::vector<Token> tokens, size_t *i)
{
	ServerConfig *server = static_cast<ServerConfig *>(dest);

	size_t j = *i;
	const std::string requestUri = tokens.at(j).getWord();

	if (!ParserHelper::isValidUri(requestUri))
		throw UriException(requestUri);

	LocationConfig route(requestUri);

	j += 2; // skip opening brace

	while (tokens.at(j).getType() != Token::CLOSE_BRACE)
	{
		for (size_t n = 0; n < LOC_FUNC_N; n++)
		{
			if (tokens.at(j).getWord() == locationContextFuncs[n].key)
			{
				j++;
				if (tokens.at(j).getType() == Token::WORD)
				{
					(this->*(locationContextFuncs[n].func))(&route, tokens, &j);
					break;
				}
				else
				{
					throw InvalidTokenException("Expected word after: " + tokens.at(j - 1).getWord());
				}
			}
		}
		j++;
	}

	route.checkMissingDirective();

	if (route.hasCgiScript())
	{
		checkCgiScript(route);
	}

	server->setLocationsConfig(route);
	(*i) = j;
}
