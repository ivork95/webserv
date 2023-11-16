#include "Client.hpp"

// constructor
Client::Client(const Server &server) : m_server(server), m_request(*this), m_timer(*this)
{
    m_socketFd = accept(m_server.m_socketFd, (struct sockaddr *)&m_remoteaddr, &m_addrlen);
    if (m_socketFd == -1)
        throw std::system_error(errno, std::generic_category(), "accept()");

    if (Helper::setNonBlocking(m_socketFd) == -1)
        throw std::system_error(errno, std::generic_category(), "fcntl()");

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
    if (nbytes <= 0)
    {
        multiplexer.modifyEpollEvents(nullptr, 0, m_socketFd);
        epoll_ctl(multiplexer.m_epollfd, EPOLL_CTL_DEL, m_socketFd, NULL);
        multiplexer.modifyEpollEvents(nullptr, 0, m_timer.m_socketFd);
        epoll_ctl(multiplexer.m_epollfd, EPOLL_CTL_DEL, m_timer.m_socketFd, NULL);

        auto it = std::find_if(multiplexer.m_clients.begin(), multiplexer.m_clients.end(), [this](Client *client)
                               { return client->m_socketFd == this->m_socketFd; });
        if (it != multiplexer.m_clients.end())
            multiplexer.m_clients.erase(it);
        else
            spdlog::error("Could not find client in m_clients");

        delete this;
        return;
    }

    if (m_request.tokenize(buf, nbytes))
        return;

    epoll_ctl(multiplexer.m_epollfd, EPOLL_CTL_DEL, m_timer.m_socketFd, NULL);
    // if (close(m_timer.m_socketFd) == -1)
    //     throw StatusCodeException(500, "close()", errno);

    try
    {
        if (!m_request.parse())
        {
            if (multiplexer.modifyEpollEvents(this, EPOLLOUT, m_socketFd))
                throw StatusCodeException(500, "delete()", errno);
        }
    }
    catch (const StatusCodeException &e)
    {
        std::cerr << e.what() << '\n';

        m_request.m_response.m_statusCode = e.getStatusCode();
        if (multiplexer.modifyEpollEvents(this, EPOLLOUT, this->m_socketFd))
            throw StatusCodeException(500, "modifyEpollEvents()", errno);
    }
}