#include "TcpServer.hpp"
#include <fcntl.h>

// port constructor
TcpServer::TcpServer(unsigned int port)
{

    int on{1};

    m_serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (m_serverSocket < 0)
    {
        std::perror("socket() failed");
        throw std::runtime_error("Error: socket() failed\n");
    }

    if (setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&on), sizeof(on)) < 0)
    {
        std::perror("setsockopt() failed");
        throw std::runtime_error("setsockopt() failed");
    }

    // fcntl(m_serverSocket, F_SETFL, O_NONBLOCK);

    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_port = htons(port);
    m_serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(m_serverSocket, (struct sockaddr *)&m_serverAddr, sizeof(m_serverAddr)) < 0)
    {
        std::perror("bind() failed");
        throw std::runtime_error("bind() failed");
    }

    if (listen(m_serverSocket, 32) < 0)
    {
        std::perror("listen() failed");
        throw std::runtime_error("Error: listen() failed\n");
    }

    std::cout << *this << " port constructor called\n";
}

// destructor
TcpServer::~TcpServer(void)
{
    std::cout << *this << " destructor called\n";

    close(m_serverSocket);
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const TcpServer &tcpserver)
{
    out << "TcpServer(" << tcpserver.m_serverSocket << ", " << tcpserver.m_serverAddr.sin_family << ", " << ntohs(tcpserver.m_serverAddr.sin_port) << ", " << ntohl(tcpserver.m_serverAddr.sin_addr.s_addr) << ')';

    return out;
}
