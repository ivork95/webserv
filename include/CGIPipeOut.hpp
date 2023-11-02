#ifndef CGIPIPEOUT_HPP
#define CGIPIPEOUT_HPP

#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <iostream>

#include "Client.hpp"
#include "Request.hpp"

#define READ 0
#define WRITE 1

class CGIPipeOut : public ASocket
{
public:
    Client &m_client;
    Request &m_request;
    Response &m_response;
    int m_pipeFd[2]{};

    // default constructor

    // constructor
    CGIPipeOut(Client &client, Request &request, Response &response);

    // copy constructor

    // copy assignment operator

    // destructor

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const CGIPipeOut &cgipipeout);

    // member functions
    void forkCloseDupExec(std::vector<ASocket *> &toBeDeleted);
};

#endif