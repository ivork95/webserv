#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "Socket.hpp"
#include "HttpMessage.hpp"

class Client : public Socket
{
public:
    struct sockaddr_storage m_remoteaddr
    {
    };
    socklen_t m_addrlen{sizeof(m_remoteaddr)};
    HttpMessage requestMessage;
    // default constructor
    Client(void) = delete;

    // serverSocket constructor
    Client(int serverSocket);

    // destructor
    ~Client(void);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const Client &clientsocket);
};

#endif
