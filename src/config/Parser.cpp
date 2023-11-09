
#include "Parser.hpp"

#include <filesystem>

/**
 * CONSTRUCTORS / DESTRUCTORS
 */
Parser::Parser(void)
{
}

/**
 * STATIC FUNCTIONS
 */
static int checkDuplicateRequestUri(std::vector<std::string> &usedRequestUris, const std::string &requestUri)
{
    if (std::find(usedRequestUris.begin(), usedRequestUris.end(), requestUri) != usedRequestUris.end())
        return 1;

    usedRequestUris.push_back(requestUri);
    return 0;
}

/**
 * MEMBER FUNCTIONS
 */
ServerConfig Parser::parseTokens(ServerConfig server)
{
    Parser parser;

    for (size_t i = 0; i < server.getTokens().size(); i++)
    {
        parser._parseServerContext(&server, server.getTokens(), &i);
    }

    server.checkMissingDirective();

    std::vector<std::string> usedRequestUris;
    for (size_t i = 0; i < server.getLocationsConfig().size(); i++)
    {
        const std::string requestUri = server.getLocationsConfig()[i].getRequestURI();
        if (checkDuplicateRequestUri(usedRequestUris, requestUri))
        {
            throw DuplicateRequestUriException(requestUri);
        }
    }

    return (server);
}
