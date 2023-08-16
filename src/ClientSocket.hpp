#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>

class ClientSocket
{
public:
    int m_clientSocket{};
    struct sockaddr_storage m_remoteaddr
    {
    };
    socklen_t m_addrlen{sizeof(m_remoteaddr)};

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
