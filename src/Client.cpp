#include "Client.hpp"

// constructor
Client::Client(const Server &server) : m_server(server), m_request(*this), m_timer(*this)
{
    m_socketFd = accept(m_server.m_socketFd, (struct sockaddr *)&m_remoteaddr, &m_addrlen);
    if (m_socketFd == -1)
        throw std::runtime_error("Error: accept()");

    if (Helper::setNonBlocking(m_socketFd) == -1)
        throw std::runtime_error("Error: fcntl()");

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

    std::cout << *this << " constructor called" << std::endl;
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const Client &client)
{
    out << "Client(" << client.m_socketFd << ": " << client.m_ipver << ": " << client.m_ipstr << ": " << client.m_port << ")";

    return out;
}

void Client::handleConnectedClient(std::vector<ASocket *> &toBeDeleted)
{
    Multiplexer &multiplexer = Multiplexer::getInstance();

    char buf[BUFSIZ]{}; // Buffer for client data
    int nbytes{static_cast<int>(recv(m_socketFd, buf, BUFSIZ - 1, 0))};
    if (nbytes <= 0)
    {
        if (close(m_socketFd) == -1)
            throw StatusCodeException(500, "Error: close()");
        m_socketFd = -1;
        toBeDeleted.push_back(this);

        if (close(m_timer.m_socketFd) == -1)
            throw StatusCodeException(500, "Error: close()");
        m_timer.m_socketFd = -1;

        return;
    }

    if (m_request.tokenize(buf, nbytes))
        return;

    if (close(m_timer.m_socketFd) == -1)
        throw StatusCodeException(500, "Error: close()");
    m_timer.m_socketFd = -1;

    try
    {
        if (!m_request.parse())
        {
            if (multiplexer.modifyEpollEvents(this, EPOLLOUT, m_socketFd))
                throw StatusCodeException(500, "Error: delete()");
        }
    }
    catch (const StatusCodeException &e)
    {
        std::cerr << e.what() << '\n';

        m_request.m_response.m_statusCode = e.getStatusCode();
        if (multiplexer.modifyEpollEvents(this, EPOLLOUT, this->m_socketFd))
            throw StatusCodeException(500, "Error: modifyEpollEvents()");
    }
    std::cout << m_request << std::endl;
}