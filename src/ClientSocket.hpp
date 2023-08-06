#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

class ClientSocket
{
public:
    int m_clientSocket{};
    struct sockaddr_in m_clientAddr
    {
    };
    socklen_t m_clientAddrSize{sizeof(m_clientAddr)};

    // default constructor
    ClientSocket(void);

    // destructor
    ~ClientSocket(void);
};

ClientSocket::ClientSocket(void)
{
    std::cout << "ClientSocket constructor called\n";
}

ClientSocket::~ClientSocket(void)
{
    std::cout << "ClientSocket destructor called\n";

    close(m_clientSocket);
}

#endif
