#ifndef CGIPIPEIN_HPP
#define CGIPIPEIN_HPP

#include "Client.hpp"

class CGIPipeIn : public Socket
{
public:
    Client &m_client;
    int m_pipeFd[2]{};

    // constructor
    CGIPipeIn(Client &client);
};

#endif