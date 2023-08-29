#include "../include/Client.hpp"

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

    // different fields in IPv4 and IPv6:
    if (m_remoteaddr.ss_family == AF_INET)
    { // IPv4
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)&m_remoteaddr;
        m_addr = &(ipv4->sin_addr);
        m_ipver = "IPv4";
        m_port = ntohs(ipv4->sin_port);
        // inet_ntop(AF_INET, &ipv4->sin_addr, remoteIP, sizeof remoteIP);
    }
    else
    { // IPv6
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)&m_remoteaddr;
        m_addr = &(ipv6->sin6_addr);
        m_ipver = "IPv6";
        m_port = ntohs(ipv6->sin6_port);
    }
    // convert the IP to a string and print it:
    inet_ntop(m_remoteaddr.ss_family, m_addr, m_ipstr, sizeof m_ipstr);

    std::cout << *this << " serverSocket constructor called\n";
}

// destructor
Client::~Client(void)
{
    std::cout << *this << " destructor called\n";

    close(m_socketFd);
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const Client &client)
{
    out << "Client(" << client.m_socketFd << ": " << client.m_ipver << ": " << client.m_ipstr << ": " << client.m_port << ")";

    return out;
}
