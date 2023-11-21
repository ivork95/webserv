#ifndef CGIPIPEOUT_HPP
#define CGIPIPEOUT_HPP

#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include "ASocket.hpp"
#include "Helper.hpp"
#include "StatusCodeException.hpp"

#define READ 0
#define WRITE 1

class Client;
class Request;
class Response;

class CGIPipeOut : public ASocket
{
public:
    Client &m_client;
    Request &m_request;
    Response &m_response;
    int m_pipeFd[2]{};

    // default constructor
    CGIPipeOut(void) = delete;

    // constructor
    CGIPipeOut(Client &client, Request &request, Response &response);

    ~CGIPipeOut(void);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const CGIPipeOut &cgipipeout);

    // member functions
    void readFromPipe(void);
    void forkCloseDupExec(void);
};

#endif