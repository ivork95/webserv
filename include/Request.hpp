#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <map>
#include <string>
#include <sstream>
#include <filesystem>

#include "Message.hpp"
#include "StatusCodes.hpp"
#include "Helper.hpp"
#include "ServerConfig.hpp"
#include "Response.hpp"

class Response;
class Client;

class Request : public Message
{
public:
    Client &m_client;
    LocationConfig m_locationconfig{};
    Response m_response{};

    // request
    std::vector<std::string> m_methodPathVersion{3, ""};
    std::map<std::string, std::string> m_generalHeaders{};
    std::string m_boundaryCode{};
    std::string m_fileName{};
    std::string m_body{};
    int m_statusCode{};

    // Chunked request
    bool m_isChunked{false};

    // constructor
    Request(void) = delete;

    Request(Client &client);

    // copy constructor

    // copy assignment operator overload

    // destructor
    ~Request(void);

    // getters/setters
    void methodPathVersionSet(void);
    void boundaryCodeSet(void);
    void generalHeadersSet(void);
    void fileNameSet(void);
    void bodySet(void);
    void locationconfigSet(void);
    void responsePathSet(void);

    // methods
    std::string boundaryCodeParse(const std::map<std::string, std::string> &requestHeaders);
    std::string generalHeadersParse(const std::string &boundaryCode);
    std::string fileNameParse(const std::map<std::string, std::string> &generalHeaders);
    std::string bodyParse(const std::string &boundaryCode);
    void bodyToDisk(const std::string &path);
    int tokenize(const char *buf, int nbytes);
    int parse(void);
    void isMethodAllowed(void);

    // chunk related
    void chunkHeaderReplace(void);
    void chunkHeadersParse(void);
    void chunkBodyExtract(void);
    void chunkBodyTokenize(void);
    void chunkBodySet(void);

    std::string m_rawChunkBody{};
    std::string m_chunkBody{};
    std::vector<std::string> m_chunkLine{};
    int m_totalChunkLength{};

    // directory listing related
    void directoryListingBodySet(const std::string &dirPath);
    std::string directoryListingParse(void);

	// delete method related
	int	deleteHandler(void);
	std::string buildDeleteFilePath(void);

	// updated to accept arg
	void updatedLocationConfigSet(const std::string &methodPath);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const Request &request);
};

#endif