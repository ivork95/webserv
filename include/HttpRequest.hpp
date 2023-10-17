#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

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

#include "HttpMessage.hpp"
#include "StatusCodes.hpp"
#include "Helper.hpp"
#include "ServerConfig.hpp"

class HttpRequest : public HttpMessage
{
public:
    std::map<std::string, std::string> m_generalHeaders{};
    std::vector<std::string> m_methodPathVersion{3};
    int m_client_max_body_size{999999};
    std::string m_boundaryCode{};
    std::string m_fileName{};
	
	// Chunked request
	bool m_isChunked{false};

    int m_statusCode{};
    std::string m_body{};

    ServerConfig m_serverconfig;

    LocationConfig m_locationconfig{};

    // constructor
    HttpRequest(void);

    HttpRequest(const ServerConfig &serverconfig);

    // copy constructor

    // copy assignment operator overload

    // destructor
    ~HttpRequest(void);

    // getters/setters
    void methodPathVersionSet(void);
    void boundaryCodeSet(void);
    void generalHeadersSet(void);
    void fileNameSet(void);
    void bodySet(void);

    // methods
    std::string boundaryCodeParse(const std::map<std::string, std::string> &requestHeaders);
    std::string generalHeadersParse(const std::string &boundaryCode);
    std::string fileNameParse(const std::map<std::string, std::string> &generalHeaders);
    std::string bodyParse(const std::string &boundaryCode);
    int tokenize(const char *buf, int nbytes);
    void parse(void);

	// chunk related
	void	chunkHeadersParse(void);
	void	chunkBodyExtract(void);
	void	chunkBodyTokenize(void);
	void	chunkBodyParse(size_t nbChunks, \
		std::vector<size_t> chunkLength, std::vector<std::string> chunkLine);
	void	chunkBodySet(void);

	std::string m_rawChunkBody{};
	std::string m_chunkBody{};
	std::vector<std::string> m_chunkLine{};

    // outstream operator overload
    friend std::ostream &
    operator<<(std::ostream &out, const HttpRequest &httprequest);
};

#endif