
#include "Configuration.hpp"

static int checkDuplicatePortNumbers(std::vector<std::string> &usedPorts, const std::string &portNb)
{
	if (std::find(usedPorts.begin(), usedPorts.end(), portNb) != usedPorts.end())
		return 1;
	usedPorts.push_back(portNb);

	return 0;
}

int Configuration::parseTokens(void)
{
	std::vector<std::string> usedPorts;

	for (size_t i = 0; i < serversConfig.size(); ++i)
	{
		try
		{
			serversConfig[i] = Parser::parseTokens(serversConfig[i]);
		}
		catch (const std::exception &e)
		{
			serversConfig[i].clearData();
			std::cerr << "Error: Parsing server " << i << std::endl;
			std::cerr << e.what() << std::endl;
			return 1;
		}
		const std::string portNb = serversConfig[i].getPortNb();
		if (checkDuplicatePortNumbers(usedPorts, portNb))
		{
			serversConfig[i].clearData();
			std::cerr << "Error: Duplicate port number: " << portNb << std::endl;
			return 1;
		}
	}

	return 0;
}

int Configuration::tokenizeServers(void)
{
	for (size_t i = 0; i < serversConfig.size(); ++i)
	{
		try
		{
			serversConfig[i].setTokens(Lexer::tokenizeServer(serversConfig[i].getRawData()));
		}
		catch (const std::exception &e)
		{
			serversConfig[i].clearData();
			std::cerr << "Error: Tokenizing server " << i << std::endl;
			std::cerr << e.what() << std::endl;
			return 1;
		}
	}

	return 0;
}

int Configuration::createServers(void)
{
	try
	{
		serversConfig = Lexer::createServers(this);
	}
	catch (const std::exception &e)
	{
		serverSections.clear();
		std::cerr << "Error: Creating server config\n";
		std::cerr << e.what() << std::endl;
		return 1;
	}

	if (!serverSections.empty())
		serverSections.clear();
	
	return 0;
}

int Configuration::readFile(std::ifstream &configFile)
{
	try
	{
		serverSections = Lexer::splitServers(configFile);
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: Splitting server blocks\n";
		std::cerr << e.what() << std::endl;
		configFile.close();
		return 1;
	}
	configFile.close();

	return 0;
}

int Configuration::initConfig(std::ifstream &configFile)
{
	if (readFile(configFile))
		return 1;

	if (createServers())
		return 1;

	if (tokenizeServers())
		return 1;

	if (parseTokens())
		return 1;
	
	if (!serverSections.empty())
		serverSections.clear();

	return 0;
}
