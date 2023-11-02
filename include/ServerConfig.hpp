
#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include <iostream>
#include <vector>

#include "Token.hpp"
#include "LocationConfig.hpp"
#include "ErrorPageConfig.hpp"
#include "Logger.hpp"

class ServerConfig
{
private:
    unsigned int _index;

    std::string _portNb;
    bool _hasPortNb{false};

    std::vector<std::string> _serverName;
    bool _hasServerName{false};

    int _clientMaxBodySize;
    bool _hasClientMaxBodySize{false};

    std::vector<ErrorPageConfig> _errorPagesConfig;
    bool _hasErrorPagesConfig{false};

    std::vector<LocationConfig> _locationsConfig;
    bool _hasLocationsConfig{false};

    std::string _rawData;
    std::vector<Token> _tokens;

public:
    ServerConfig(void);
    ServerConfig(const unsigned int index, const std::string rawData);
    ~ServerConfig(void);

    const unsigned int &getIndex(void) const;

    const std::string &getPortNb(void) const;
    const std::vector<std::string> &getServerName(void) const;
    const int &getClientMaxBodySize(void) const;
    const std::vector<ErrorPageConfig> &getErrorPagesConfig(void) const;
    const std::vector<LocationConfig> &getLocationsConfig(void) const;
    const std::string &getRawData(void) const;
    const std::vector<Token> &getTokens(void) const;

    void setIndex(const unsigned int &index);

    void setPortNb(const std::string &portNb);
    void setServerName(const std::vector<std::string> &serverName);
    void setClientMaxBodySize(const int &clientMaxBodySize);
    void setErrorPagesConfig(const ErrorPageConfig &errorPagesConfig);
    void setLocationsConfig(const LocationConfig &locationsConfig);
    void setRawData(const std::string &rawData);
    void setTokens(const std::vector<Token> &tokens);

    bool hasPortNb(void) const;
    bool hasServerName(void) const;
    bool hasClientMaxBodySize(void) const;
    bool hasErrorPagesConfig(void) const;
    bool hasLocationsConfig(void) const;

    void checkMissingDirective(void); // TODO make virtual?

    friend std::ostream &operator<<(std::ostream &out, const ServerConfig &server);
};

#endif