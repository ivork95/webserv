#ifndef CGIPIPEIN_HPP
#define CGIPIPEIN_HPP

#include <cstring>
#include <unistd.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include "ASocket.hpp"
#include "StatusCodeException.hpp"
#include "Helper.hpp"
#include "CGIPipeOut.hpp"

#define READ 0
#define WRITE 1

class Client;

class CGIPipeIn : public ASocket
{
public:
    Client &m_client;
    int m_pipeFd[2]{};
    const char m_input[111] = "<html>\r\n<head><title>hello.py</title></head>\r\n<body>\r\n<center><h1>Whohoo! CGI works...</h1></center>\r\n";

    // default constructor
    CGIPipeIn(void) = delete;

    // constructor
    CGIPipeIn(Client &client);

    ~CGIPipeIn(void);

    // outstream operator overload
    friend std::ostream &
    operator<<(std::ostream &out, const CGIPipeIn &cgipipein);

    // member functions
    void dupCloseWrite(void);
};

#endif