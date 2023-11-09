
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Configuration.hpp"

static int checkDuplicatePortNumbers(std::vector<std::string> &usedPorts, const std::string &portNb)
{
    if (std::find(usedPorts.begin(), usedPorts.end(), portNb) != usedPorts.end())
        return 1;
    usedPorts.push_back(portNb);

    return 0;
}

static int parseTokens(Configuration *config)
{
    std::vector<std::string> usedPorts;

    for (size_t i = 0; i < config->serversConfig.size(); ++i)
    {
        try
        {
            config->serversConfig[i] = Parser::parseTokens(config->serversConfig[i]);
        }
        catch (const std::exception &e)
        {
            return 1;
        }
        const std::string portNb = config->serversConfig[i].getPortNb();
        if (checkDuplicatePortNumbers(usedPorts, portNb))
        {
            return 1;
        }
    }

    return 0;
}

static int tokenizeServers(Configuration *config)
{
    for (size_t i = 0; i < config->serversConfig.size(); ++i)
    {
        try
        {
            config->serversConfig[i].setTokens(Lexer::tokenizeServer(config->serversConfig[i].getRawData()));
        }
        catch (const std::exception &e)
        {
            return 1;
        }
    }

    return 0;
}

static int createServers(Configuration *config)
{
    try
    {
        config->serversConfig = Lexer::createServers(config);
    }
    catch (const std::exception &e)
    {
        return 1;
    }

    return 0;
}

static int readFile(std::ifstream &configFile, Configuration *config)
{
    try
    {
        config->serverSections = Lexer::splitServers(configFile);
    }
    catch (const std::exception &e)
    {
        return 1;
    }

    return 0;
}

static int openFile(std::ifstream &configFile, const std::string &filePath)
{
    configFile.open(filePath);
    if (!configFile.is_open())
    {
        return 1;
    }

    return 0;
}

static bool isValidConfigExtension(const std::string &str)
{
    const std::string inputFileExtension = std::filesystem::path(str).extension();

    if (inputFileExtension != ".conf")
    {
        return false;
    }

    return true;
}

int initConfig(const std::string &filePath, Configuration &config)
{
    std::ifstream configFile;

    // check config file extension
    if (!isValidConfigExtension(filePath))
        return 1;

    // open file for read
    if (openFile(configFile, filePath))
        return 1;

    // read file, check braces & split sections
    if (readFile(configFile, &config))
        return 1;

    // close file
    configFile.close();

    // create Server objects from server sections
    if (createServers(&config))
        return 1;

    // tokenize each server
    if (tokenizeServers(&config))
        return 1;

    // parse tokens into data structures
    if (parseTokens(&config))
        return 1;

    return 0;
}
