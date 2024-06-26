#include "Client.hpp"

// constructor
Client::Client(const Server &server) : m_server(server), m_request(*this), m_timer(*this)
{
    m_socketFd = accept(m_server.m_socketFd, (struct sockaddr *)&m_remoteaddr, &m_addrlen);
    if (m_socketFd == -1)
        throw std::system_error(errno, std::generic_category(), "accept()");

    if (Helper::setNonBlocking(m_socketFd) == -1)
    {
        close(m_socketFd);
        throw std::system_error(errno, std::generic_category(), "fcntl()");
    }

    // different fields in IPv4 and IPv6:
    if (m_remoteaddr.ss_family == AF_INET)
    { // IPv4
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)&m_remoteaddr;
        m_addr = &(ipv4->sin_addr);
        m_ipver = "IPv4";
        m_port = ntohs(ipv4->sin_port);
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

    spdlog::debug("{} constructor", *this);
}

Client::~Client(void)
{
    close(m_socketFd);

    spdlog::debug("{} destructor", *this);
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const Client &client)
{
    out << "Client(" << client.m_socketFd << ": " << client.m_ipver << ": " << client.m_ipstr << ": " << client.m_port << ")";

    return out;
}

void Client::handleConnectedClient()
{
    Multiplexer &multiplexer = Multiplexer::getInstance();

    char buf[BUFSIZ]{}; // Buffer for client data
    int nbytes{static_cast<int>(recv(m_socketFd, buf, BUFSIZ - 1, 0))};
    spdlog::info("Received message from {}:\n{}\n", *this, buf);
    if (nbytes <= 0)
    {
        multiplexer.removeFromEpoll(m_socketFd);
        multiplexer.removeFromEpoll(m_timer.m_socketFd);
        multiplexer.removeClientBySocketFd(this);
        return;
    }
    if (m_request.tokenize(buf, nbytes))
        return;
    spdlog::info("Message complete {}:\n{}\n", *this, m_request.m_rawMessage);
    multiplexer.removeFromEpoll(m_timer.m_socketFd);
    if (!m_request.parse())
    {
        if (multiplexer.modifyEpollEvents(this, EPOLLOUT, m_socketFd))
            throw StatusCodeException(500, "delete()", errno);
    }
}