#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <fcntl.h>
#include <iostream>

class Socket
{
public:
    int m_socketFd;
    int setNonBlocking(void);
    virtual ~Socket(void)
    {
        std::cout << "Socket destructor called\n";
    }
};

#endif
