#ifndef CGIPIPEOUT_HPP
#define CGIPIPEOUT_HPP

#include <sys/wait.h>

#include "Client.hpp"
#include "Request.hpp"

#define READ 0
#define WRITE 1

class CGIPipeOut : public Socket
{
public:
    Client &m_client;
    Request &m_request;
    Response &m_response;
    int m_pipeFd[2]{};

    // constructor
    CGIPipeOut(Client &client, Request &request, Response &response);
};

#endif