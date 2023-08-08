#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>

class ClientSocket
{
public:
    int m_clientSocket{};
    struct sockaddr_in m_clientAddr
    {
    };
    socklen_t m_clientAddrSize{sizeof(m_clientAddr)};

    // default constructor
    ClientSocket(void) = delete;

    // serverSocket constructor
    ClientSocket(int serverSocket);

    // destructor
    ~ClientSocket(void);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const ClientSocket &clientsocket);
};

#endif
