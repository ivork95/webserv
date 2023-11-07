
#include "Lexer.hpp"

/**
 * CONSTRUCTORS / DESTRUCTORS
 */
Lexer::Lexer(void)
{
    std::cout << "Lexer default constructor called\n";
}

/**
 * STATIC FUNCTIONS
 */
static bool isSpecialChar(const char &c)
{
    return (c == '{' || c == '}' || c == ';' || c == '\n');
}

static bool isNotSpecialChar(const char &c)
{
    return (c != '{' && c != '}' && c != ';' && c != '\n');
}

/**
 * MEMBER FUNCTIONS
 */
void Lexer::_splitLine(std::vector<Token> *tokens, std::string &line)
{
    std::string::iterator it = line.begin();

    // Loop until line end, skipping whitespaces
    while (it != line.end())
    {
        while (isspace(*it))
            it++;

        // Get type of current char (word, {, } or ;)
        if (isSpecialChar(*it))
        {
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
        }
        else
        {
            std::string word;

            // Loop until reaching another space, special char or end to have a full word
            while (!isspace(*it) && isNotSpecialChar(*it) && it != line.end())
            {
                word += *it;
                it++;
            }

            // Add word to tokens
            Token token(word);
            tokens->push_back(token);
        }
    }
}

std::vector<Token> Lexer::tokenizeServer(const std::string &rawData)
{
    std::vector<Token> tokens;
    std::istringstream linesStream(rawData);
    std::string line;

    while (std::getline(linesStream, line))
    {
        // std::cout << "[" << line << "]" << std::endl; // ? debug
        if (line.empty())
        {
            continue;
        }
        else if (line.back() != ';' && line.back() != '{' && line.back() != '}')
        {
            throw InvalidTokenException("Line does not end with ;, { or } [" + line + "]");
        }
        _splitLine(&tokens, line);
    }

    return (tokens);
}

std::vector<ServerConfig> Lexer::createServers(Configuration *config)
{
    std::vector<ServerConfig> servers;
    if (config->serverSections.empty())
    {
        throw NoServerSectionException();
        // std::cout << "Error: No server sections found" << std::endl;
        // return std::vector<ServerConfig>();
    }
    for (size_t i = 0; i < config->serverSections.size(); ++i)
    {
        ServerConfig server(i, config->serverSections[i]);
        servers.push_back(server);
    }
    if (servers.empty())
    {
        throw NoServerBlockException();
        // return std::vector<ServerConfig>();
    }
    return (servers);
}

std::vector<std::string> Lexer::splitServers(std::ifstream &configFile)
{
    std::string content((std::istreambuf_iterator<char>(configFile)),
                        (std::istreambuf_iterator<char>()));
    std::vector<std::string> sections;
    size_t startPos = 0;
    std::stack<char> braceStack;

    if (content.empty())
        throw EmptyFileException();
    for (size_t i = 0; i < content.size(); ++i)
    {
        if (content[i] == '{')
        {
            braceStack.push('{');
            if (braceStack.size() == 1)
            {
                startPos = i;
            }
        }
        else if (content[i] == '}')
        {
            if (!braceStack.empty())
            {
                braceStack.pop();
                if (braceStack.empty())
                {
                    sections.push_back(content.substr(startPos, i - startPos + 1));
                }
            }
            else
            {
                throw UnmatchedBracesException();
            }
        }
    }
    if (!braceStack.empty())
        throw UnmatchedBracesException();
    if (sections.empty())
        throw NoServerSectionException();
    return (sections);
}