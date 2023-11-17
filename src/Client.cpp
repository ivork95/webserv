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

// bool alpha(void)
// {
//     char buf[BUFSIZ]{};

//     int nbytes{static_cast<int>(recv(m_socketFd, buf, BUFSIZ - 1, 0))};
//     if (nbytes <= 0)
//     {
//         multiplexer.modifyEpollEvents(nullptr, 0, m_socketFd);
//         epoll_ctl(multiplexer.m_epollfd, EPOLL_CTL_DEL, m_socketFd, NULL);

//         multiplexer.modifyEpollEvents(nullptr, 0, m_timer.m_socketFd);
//         epoll_ctl(multiplexer.m_epollfd, EPOLL_CTL_DEL, m_timer.m_socketFd, NULL);

//         multiplexer.removeClientBySocketFd(this->m_socketFd);
//         delete this;

//         return true;
//     }

//     m_rawMessage.append(buf, buf + nbytes);

//     size_t fieldLinesEndPos = m_rawMessage.find("\r\n\r\n");
//     if (fieldLinesEndPos == std::string::npos)
//         return false;

//     if (m_requestHeaders.empty())
//         requestHeadersSet();

//     if (m_requestHeaders.contains("Content-Length"))
//     {
//         if (!m_isContentLengthConverted)
//             contentLengthSet();
//         if (m_contentLength > static_cast<int>((m_rawMessage.length() - (fieldLinesEndPos + 4))))
//             return false;
//     }
//     else if (m_requestHeaders.contains("Transfer-Encoding"))
//     {
//         if (!m_isChunked)
//             m_isChunked = true;
//         size_t chunkEndPos = m_rawMessage.find("\r\n0\r\n\r\n");
//         if (chunkEndPos == std::string::npos)
//             return false;
//     }

//     return true;
// }

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

        multiplexer.removeClientBySocketFd(this->m_socketFd);
        delete this;

        return;
    }

    if (m_request.tokenize(buf, nbytes))
        return;

    if (epoll_ctl(multiplexer.m_epollfd, EPOLL_CTL_DEL, m_timer.m_socketFd, NULL) == -1)
        throw StatusCodeException(500, "epoll_ctl()", errno);

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

        m_request.m_response.statusCodeSet(e.getStatusCode());
        if (multiplexer.modifyEpollEvents(this, EPOLLOUT, this->m_socketFd))
            throw StatusCodeException(500, "modifyEpollEvents()", errno);
    }
}