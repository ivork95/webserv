#include "../include/TcpServer.hpp"

// port constructor
TcpServer::TcpServer(const char *port)
{
    int yes{1}; // For setsockopt() SO_REUSEADDR, below
    int rv{};
    struct addrinfo *ai{};
    struct addrinfo *p{};

    // Get us a socket and bind it
    std::memset(&m_hints, 0, sizeof(m_hints));
    m_hints.ai_family = AF_UNSPEC;
    m_hints.ai_socktype = SOCK_STREAM;
    m_hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, port, &m_hints, &ai)) != 0)
    {
        std::cerr << "selectserver: " << gai_strerror(rv) << '\n';
        throw std::runtime_error("Error: getaddrinfo() failed\n");
    }

    for (p = ai; p != NULL; p = p->ai_next)
    {
        m_socketFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (m_socketFd < 0)
        {
            continue;
        }

        // Lose the pesky "address already in use" error message
        setsockopt(m_socketFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(m_socketFd, p->ai_addr, p->ai_addrlen) < 0)
        {
            close(m_socketFd);
            continue;
        }

        break;
    }

    // get the pointer to the address itself,
    // different fields in IPv4 and IPv6:
    if (p->ai_family == AF_INET)
    { // IPv4
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
        m_addr = &(ipv4->sin_addr);
        m_ipver = "IPv4";
        m_port = ntohs(ipv4->sin_port);
    }
    else
    { // IPv6
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
        m_addr = &(ipv6->sin6_addr);
        m_ipver = "IPv6";
        m_port = ntohs(ipv6->sin6_port);
    }
    // convert the IP to a string and print it:
    inet_ntop(p->ai_family, m_addr, m_ipstr, sizeof m_ipstr);

    freeaddrinfo(ai); // All done with this

    // If we got here, it means we didn't get bound
    if (p == NULL)
    {
        std::perror("bind() failed");
        throw std::runtime_error("bind() failed");
    }

    // Listen
    if (listen(m_socketFd, 10) == -1)
    {
        std::perror("listen() failed");
        throw std::runtime_error("Error: listen() failed\n");
    }

    setNonBlocking();

    std::cout << *this << " port constructor called\n";
}

// destructor
TcpServer::~TcpServer(void)
{
    std::cout << *this << " destructor called\n";

    close(m_socketFd);
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const TcpServer &tcpserver)
{
    out << "TcpServer(" << tcpserver.m_socketFd << ": " << tcpserver.m_ipver << ": " << tcpserver.m_ipstr << ": " << tcpserver.m_port << ")";

    return out;
}
