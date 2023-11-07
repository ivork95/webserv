#ifndef CGIPIPEIN_HPP
#define CGIPIPEIN_HPP

#include <cstring>
#include <unistd.h>

#include "ASocket.hpp"
#include "StatusCodeException.hpp"
#include "Helper.hpp"
#include "Client.hpp"

#define READ 0
#define WRITE 1

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

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const CGIPipeIn &cgipipein);

    // member functions
    void dupCloseWrite(std::vector<ASocket *> &toBeDeleted);
};

#endif
