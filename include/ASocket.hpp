#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "Logger.hpp"

class ASocket
{
public:
    int m_socketFd{};

    // data only used to print info about socket
    char m_ipstr[INET6_ADDRSTRLEN]{};
    void *m_addr{};
    const char *m_ipver{};
    int m_port{};

    // default constructor

    // constructor

    // copy constructor

    // copy assignment operator overload

    // destructor
    virtual ~ASocket(void) = 0;
};

#endif