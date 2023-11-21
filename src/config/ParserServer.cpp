
#include "Parser.hpp"

/**
 * STATIC FUNCTIONS
 */
t_parseFuncKey Parser::serverContextFuncs[SERVER_FUNC_N] = {
	{"listen", &Parser::_parseListen},
	{"server_name", &Parser::_parseServerName},
	{"error_page", &Parser::_parseErrorPage},
	{"client_max_body_size", &Parser::_parseServerClientMaxBodySize},
	{"location", &Parser::_parseLocationContext},
};

/**
 * MEMBER FUNCTIONS
 */
void Parser::_parseServerClientMaxBodySize(void *dest, std::vector<Token> tokens, size_t *i)
{
	ServerConfig *server = static_cast<ServerConfig *>(dest);

	const std::string rawValue = tokens.at(*i).getWord();
	server->setClientMaxBodySize(ParserHelper::parseClientMaxBodySize(rawValue));
}

void Parser::_parseErrorPage(void *dest, std::vector<Token> tokens, size_t *i)
{
	ServerConfig *server = static_cast<ServerConfig *>(dest);

	size_t j = *i;
	std::vector<int> errorCodes;

	while (tokens.at(j).getType() == Token::WORD && ParserHelper::isNumber(tokens.at(j).getWord()))
	{
		int errorCode = std::atoi(tokens.at(j).getWord().c_str());
		if (ParserHelper::isValidErrorCode(errorCode))
		{
			errorCodes.push_back(errorCode);
		}
		else
		{
			throw ErrorCodeException(tokens.at(j).getWord());
		}
		j++;
	}

	if (errorCodes.empty())
		throw ErrorCodeException(tokens.at(j).getWord());

	const std::string filePath = tokens.at(j).getWord();
	if (!ParserHelper::isValidPath(filePath, false))
		throw PathException(filePath);

	std::filesystem::perms requiredPermissions = OWREAD | OTREAD | GRREAD;
	if (!ParserHelper::hasRequiredPermissions(filePath, requiredPermissions))
		throw MissingPermissionsException(filePath + " (r required)");

	ErrorPageConfig errorPage(errorCodes, filePath);

	server->setErrorPagesConfig(errorPage);
	(*i) = j;
}

void Parser::_parseServerName(void *dest, std::vector<Token> tokens, size_t *i)
{
	ServerConfig *server = static_cast<ServerConfig *>(dest);

	size_t j = *i;
	std::vector<std::string> serverNames;

	while (tokens.at(j).getType() == Token::WORD)
	{
		const std::string serverName = tokens.at(j).getWord();

		if (!ParserHelper::isValidServerName(serverName))
		{
			throw ServerNameException(serverName);
		}

		if (std::find(serverNames.begin(), serverNames.end(), serverName) != serverNames.end())
		{
			throw DuplicateServerNameException(serverName);
		}

		serverNames.push_back(serverName);
		j++;
	}
	server->setServerName(serverNames);
}

void Parser::_parseListen(void *dest, std::vector<Token> tokens, size_t *i)
{
	ServerConfig *server = static_cast<ServerConfig *>(dest);

	const std::string portNumber = tokens.at(*i).getWord();
	if (!ParserHelper::isValidPortNumber(portNumber))
		throw PortNumberException(portNumber);

	server->setPortNb(portNumber);
}

void Parser::_parseServerContext(ServerConfig *server, std::vector<Token> tokens, size_t *i)
{
	for (size_t n = 0; n < SERVER_FUNC_N; n++)
	{
		if (tokens.at(*i).getWord() == serverContextFuncs[n].key)
		{
			(*i)++;
			if (tokens.at(*i).getType() == Token::WORD)
			{
				(this->*(serverContextFuncs[n].func))(server, tokens, i);
				break;
			}
			else
			{
				throw InvalidTokenException("Expected word after: " + tokens.at(*i - 1).getWord());
			}
		}
	}
}
