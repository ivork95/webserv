#include "ClientSocket.hpp"

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
