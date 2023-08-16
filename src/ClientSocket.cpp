#include "ClientSocket.hpp"

// Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// serverSocket constructor
ClientSocket::ClientSocket(int serverSocket)
{
    char remoteIP[INET6_ADDRSTRLEN];

    m_clientSocket = accept(serverSocket, (struct sockaddr *)&m_remoteaddr, &m_addrlen);
    if (m_clientSocket == -1)
    {
        std::perror("accept() failed");
        throw std::runtime_error("Error: accept() failed\n");
    }

    std::cout << "pollserver: new connection from " << inet_ntop(m_remoteaddr.ss_family, get_in_addr((struct sockaddr *)&m_remoteaddr), remoteIP, INET6_ADDRSTRLEN) << " on socket " << m_clientSocket << '\n';
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
    (void)clientsocket;
    out << "ClientSocket(" << ')';

    return out;
}
