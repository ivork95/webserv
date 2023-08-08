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
    ClientSocket(void) = delete;

    // serverSocket constructor
    ClientSocket(int serverSocket);

    // destructor
    ~ClientSocket(void);

    // outstream operator overload
    friend std::ostream &operator<<(std::ostream &out, const ClientSocket &clientsocket);
};

// serverSocket constructor
ClientSocket::ClientSocket(int serverSocket)
{
    m_clientSocket = accept(serverSocket, (struct sockaddr *)&(m_clientAddr), &(m_clientAddrSize));
    if (m_clientSocket == -1)
    {
        std::perror("accept() failed");
        throw std::runtime_error("Error: accept() failed\n");
    }

    std::cout << *this << " constructor called\n";
}

// destructor
ClientSocket::~ClientSocket(void)
{
    std::cout << *this << " destructor called\n";

    close(m_clientSocket);
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const ClientSocket &clientsocket)
{
    out << "ClientSocket(" << clientsocket.m_clientSocket << ", " << clientsocket.m_clientAddr.sin_family << ", " << ntohs(clientsocket.m_clientAddr.sin_port) << ", " << ntohl(clientsocket.m_clientAddr.sin_addr.s_addr) << ')';

    return out;
}

#endif
