#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <map>
#include <string>
#include <sstream>
#include <filesystem>

#include "Message.hpp"
#include "StatusCodeException.hpp"
#include "Helper.hpp"
#include "ConfigServer.hpp"
#include "Response.hpp"
#include "CGIPipeIn.hpp"
#include "CGIPipeOut.hpp"

class Response;
class Client;

class Request : public Message
{
private:
    Client &m_client;
    Response m_response{};
    CGIPipeIn m_pipein;
    CGIPipeOut m_pipeout;
    LocationConfig m_locationconfig{};

    std::map<std::string, std::string> m_generalHeaders{};
    std::vector<std::string> m_methodPathVersion{3, ""};
    std::vector<std::string> m_query{};
    std::vector<std::string> m_chunkLine{};
    std::string m_boundaryCode{};
    std::string m_fileName{};
    std::string m_body{};
    std::string m_rawChunkBody{};
    std::string m_chunkBody{};
    bool m_isChunked{false};
    int m_totalChunkLength{};

public:
    // constructor
    Request(void) = delete;

    // constructor
    Request(Client &client);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const Request &request);

    // getters/setters
    Response &getResponse(void) { return m_response; }
    CGIPipeOut &getPipeOut(void) { return m_pipeout; }
    std::string &getBody(void) { return m_body; }
    LocationConfig &getLocationConfig(void) { return m_locationconfig; }
    std::vector<std::string> &getQuery(void) { return m_query; }

    void methodPathVersionSet(void);
    void boundaryCodeSet(void);
    void generalHeadersSet(void);
    void fileNameSet(void);
    void bodySet(void);
    void locationConfigSet(const std::string &methodPath);
    void responsePathSet(void);

    // methods
    std::string boundaryCodeParse(const std::map<std::string, std::string> &requestHeaders);
    std::string generalHeadersParse(const std::string &boundaryCode);
    std::string fileNameParse(const std::map<std::string, std::string> &generalHeaders);
    std::string bodyParse(const std::string &boundaryCode);
    void bodyToDisk(const std::string &path);
    void isMethodAllowed(void);
    int tokenize(const char *buf, int nbytes);
    int parse(void);

    // chunk related
    void chunkHeaderReplace(void);
    void chunkHeadersParse(void);
    void chunkBodyExtract(void);
    void chunkBodyTokenize(void);
    void chunkBodySet(void);
    int chunkHandler(void);

    // get method related
    int getHandler(void);
    int uploadHandler(void);

    // directory listing related
    void directoryListingBodySet(const std::string &dirPath);
    std::string directoryListingParse(void);

    // delete method related
    int deleteHandler(void);
    std::string buildDeleteFilePath(void);

    // post method related
    int postHandler(void);
};

#endif