#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>

class Socket
{
public:
    int m_socketFd;

    // data only used to print info about socket
    char m_ipstr[INET6_ADDRSTRLEN];
    void *m_addr;
    const char *m_ipver;
    int m_port;

    int setNonBlocking(void);
    virtual ~Socket(void)
    {
        std::cout << "Socket destructor called\n";
    }
};

#endif