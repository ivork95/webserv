#ifndef CGIPIPEOUT_HPP
#define CGIPIPEOUT_HPP

#include "Client.hpp"
#include "Request.hpp"

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