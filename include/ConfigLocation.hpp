
#ifndef CONFIG_LOCATION_HPP
#define CONFIG_LOCATION_HPP

#include "Exceptions.hpp"

#include <iostream>

#include <vector>
#include <map>

class LocationConfig
{
private:
	std::string _requestURI;
	bool _hasRequestURI{false};

	std::string _rootPath;
	bool _hasRootPath{false};

	int _clientMaxBodySize;
	bool _hasClientMaxBodySize{false};

	bool _autoIndex;
	bool _hasAutoIndex{false};

	std::vector<std::string> _indexFile;
	bool _hasIndexFile{false};

	std::string _cgiScript;
	bool _hasCgiScript{false};

	std::string _cgiInterpreter;
	bool _hasCgiInterpreter{false};

	std::string _absCgiScript;
	bool _hasAbsCgiScript{false};

	std::vector<std::string> _httpMethods;
	bool _hasHttpMethods{false};

public:
	// default constructor
	LocationConfig(void);

	// constructor
	LocationConfig(const std::string &requestURI);

	// copy constructor
	LocationConfig(const LocationConfig &other);

	const std::string &getRequestURI(void) const;
	const std::string &getRootPath(void) const;
	const int &getClientMaxBodySize(void) const;
	const bool &getAutoIndex(void) const;
	const std::vector<std::string> &getIndexFile(void) const;
	const std::string &getCgiScript(void) const;
	const std::string &getCgiInterpreter(void) const;
	const std::string &getAbsCgiScript(void) const;
	const std::vector<std::string> &getHttpMethods(void) const;

	void setRequestURI(const std::string &requestURI);
	void setRootPath(const std::string &rootPath);
	void setClientMaxBodySize(const int &clientMaxBodySize);
	void setAutoIndex(const bool &autoIndex);
	void setIndexFile(const std::vector<std::string> &indexFile);
	void setCgiScript(const std::string &cgiScript);
	void setCgiInterpreter(const std::string &cgiInterpreter);
	void setAbsCgiScript(const std::string &cgiScriptPath);
	void setHttpMethods(const std::vector<std::string> &httpMethods);

	bool hasRequestURI(void) const;
	bool hasRootPath(void) const;
	bool hasClientMaxBodySize(void) const;
	bool hasAutoIndex(void) const;
	bool hasIndexFile(void) const;
	bool hasCgiScript(void) const;
	bool hasCgiInterpreter(void) const;
	bool hasAbsCgiScript(void) const;
	bool hasHttpMethods(void) const;

	void checkMissingDirective(void);

	// operator overloads
	LocationConfig &operator=(const LocationConfig &other);
	friend std::ostream &operator<<(std::ostream &out, const LocationConfig &route);
};

#endif