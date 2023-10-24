#ifndef CGIPIPEIN_HPP
#define CGIPIPEIN_HPP

#include "Client.hpp"
#include <cstring>

class CGIPipeIn : public Socket
{
public:
    Client &m_client;
    int m_pipeFd[2]{};
    const char m_input[111] = "<html>\r\n<head><title>hello.py</title></head>\r\n<body>\r\n<center><h1>Whohoo! CGI works...</h1></center>\r\n";

    // constructor
    CGIPipeIn(Client &client);
};

#endif