#include "Client.hpp"

// Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// Get sockport, IPv4 or IPv6:
int get_in_port(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)sa;
        return ntohs(ipv4->sin_port);
    }

    struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)sa;
    return ntohs(ipv6->sin6_port);
}

// serverSocket constructor
Client::Client(int serverSocket)
{
    m_socketFd = accept(serverSocket, (struct sockaddr *)&m_remoteaddr, &m_addrlen);
    if (m_socketFd == -1)
    {
        std::perror("accept() failed");
        throw std::runtime_error("Error: accept() failed\n");
    }

    setNonBlocking();

    char remoteIP[INET6_ADDRSTRLEN];
    int port = get_in_port((struct sockaddr *)&m_remoteaddr);
    std::cout << "Client(" << m_socketFd << ": " << inet_ntop(m_remoteaddr.ss_family, get_in_addr((struct sockaddr *)&m_remoteaddr), remoteIP, INET6_ADDRSTRLEN) << ": " << port << ")\n";
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
