#ifndef CLIENT_HPP
#define CLIENT_HPP

class Client
{
public:
    int m_clientSocket{};
    struct sockaddr_in m_clientAddr{};
    socklen_t m_clientAddrSize{sizeof(m_clientAddr)};

    // default constructor
    Client(void);

    // destructor
    ~Client(void);
};

Client::Client(void)
{
    std::cout << "Client constructor called\n";
}

Client::~Client(void)
{
    std::cout << "Client destructor called\n";
    close(m_clientSocket);
}

#endif
