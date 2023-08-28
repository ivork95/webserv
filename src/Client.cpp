#include "../includes/Client.hpp"

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
Client::Client(int serverSocket)
{
    char remoteIP[INET6_ADDRSTRLEN];

    m_socketFd = accept(serverSocket, (struct sockaddr *)&m_remoteaddr, &m_addrlen);
    if (m_socketFd == -1)
    {
        std::perror("accept() failed");
        throw std::runtime_error("Error: accept() failed\n");
    }

    setNonBlocking();

    std::cout << "pollserver: new connection from " << inet_ntop(m_remoteaddr.ss_family, get_in_addr((struct sockaddr *)&m_remoteaddr), remoteIP, INET6_ADDRSTRLEN) << " on socket " << m_socketFd << '\n';
}

// destructor
Client::~Client(void)
{
    std::cout << *this << " destructor called\n";

    close(m_socketFd);
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const Client &clientsocket)
{
    (void)clientsocket;
    out << "Client(" << ')';

    return out;
}
