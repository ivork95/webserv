#include "Server.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

// constructor
Server::Server(const ServerConfig &serverconfig) : m_serverconfig(serverconfig)
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
    if ((rv = getaddrinfo(NULL, m_serverconfig.getPortNb().c_str(), &m_hints, &ai)) != 0)
    {
        std::cerr << "selectserver: " << gai_strerror(rv) << '\n';
        throw std::system_error(errno, std::generic_category(), "getaddrinfo()");
    }

    for (p = ai; p != NULL; p = p->ai_next)
    {
        m_socketFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (m_socketFd < 0)
            continue;
        setsockopt(m_socketFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)); // Lose the pesky "address already in use" error message
        if (bind(m_socketFd, p->ai_addr, p->ai_addrlen) < 0)
        {
            close(m_socketFd);
            continue;
        }
        break;
    }

    if (p == NULL) // If we got here, it means we didn't get bound
    {
        freeaddrinfo(ai); // All done with this
        throw std::system_error(errno, std::generic_category(), "selectserver: failed to bind");
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
    inet_ntop(p->ai_family, m_addr, m_ipstr, sizeof m_ipstr); // convert the IP to a string and print it

    freeaddrinfo(ai); // All done with this

    if (listen(m_socketFd, BACKLOG) == -1)
        throw std::system_error(errno, std::generic_category(), "listen()");
    if (Helper::setNonBlocking(m_socketFd) == -1)
        throw std::system_error(errno, std::generic_category(), "fcntl()");

    Multiplexer &multiplexer = Multiplexer::getInstance();
    if (multiplexer.addToEpoll(this, EPOLLIN | EPOLLRDHUP, m_socketFd))
        throw std::system_error(errno, std::generic_category(), "addToEpoll()");

    spdlog::debug("{} constructor", *this);
}

// destructor
Server::~Server(void)
{
    close(m_socketFd);

    spdlog::debug("{} destructor", *this);
}

void Server::handleNewConnection(void) const
{
    Multiplexer &multiplexer = Multiplexer::getInstance();

    try
    {
        Client *client = new Client{*this};
        multiplexer.m_clients.push_back(client);
        if (multiplexer.addToEpoll(client, EPOLLIN | EPOLLRDHUP, client->m_socketFd))
            throw std::system_error(errno, std::generic_category(), "addToEpoll()");
        if (multiplexer.addToEpoll(&(client->getTimer()), EPOLLIN | EPOLLRDHUP, client->getTimer().m_socketFd))
            throw std::system_error(errno, std::generic_category(), "addToEpoll()");
        spdlog::info("New connection: {}", *client);
    }
    catch (const std::exception &e)
    {
        ;
    }
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const Server &server)
{
    out << "Server(" << server.m_socketFd << ": " << server.m_ipver << ": " << server.m_ipstr << ": " << server.m_port << ")";

    return out;
}