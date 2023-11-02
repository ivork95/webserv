#include "Client.hpp"

// server constructor
Client::Client(const Server &server) : m_server(server), m_request(*this), m_timer(*this)
{
    m_socketFd = accept(m_server.m_socketFd, (struct sockaddr *)&m_remoteaddr, &m_addrlen);
    if (m_socketFd == -1)
        throw std::runtime_error("Error: accept()\n");

    if (Helper::setNonBlocking(m_socketFd) == -1)
        throw std::runtime_error("Error: fcntl()\n");

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

    // spdlog::debug("{0} constructor called", *this);
    Logger::getInstance().debug("Client(" + std::to_string(m_socketFd) + ": " + m_ipver + ": " + m_ipstr + ": " + std::to_string(m_port) + ") constructor called");
}

// destructor
Client::~Client(void)
{
    // spdlog::debug("{0} destructor called", *this);
    Logger::getInstance().debug("Client(" + std::to_string(m_socketFd) + ": " + m_ipver + ": " + m_ipstr + ": " + std::to_string(m_port) + ") destructor called");
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
        close(m_socketFd);
        m_socketFd = -1;
        toBeDeleted.push_back(this);

        close(m_timer.m_socketFd);
        m_timer.m_socketFd = -1;

        return;
    }

    if (m_request.tokenize(buf, nbytes))
        return;

    close(m_timer.m_socketFd);
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
        // spdlog::warn(e.what());
        Logger::getInstance().debug(e.what());

        m_request.m_response.m_statusCode = e.getStatusCode();
        if (multiplexer.modifyEpollEvents(this, EPOLLOUT, this->m_socketFd))
            throw StatusCodeException(500, "Error: modifyEpollEvents()");
    }
    // spdlog::critical(m_request);
    std::ostringstream request;
    request << m_request;
    Logger::getInstance().debug(request.str()); // TODO fix this
}