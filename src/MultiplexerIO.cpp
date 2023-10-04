#include "MultiplexerIO.hpp"
#include "HttpResponse.hpp"

#define BUFSIZE 256

// default constructor
MultiplexerIO::MultiplexerIO(void)
{
    m_epollfd = epoll_create(1);
    if (m_epollfd == -1)
    {
        std::perror("epoll_create() failed");
        throw std::runtime_error("Error: epoll_create() failed\n");
    }
    spdlog::debug("{} default constructor called", *this);
}

// destructor
MultiplexerIO::~MultiplexerIO(void)
{
    spdlog::debug("{} destructor called", *this);

    close(m_epollfd);
}

// member functions

void MultiplexerIO::run(std::vector<TcpServer *> servers)
{
    // std::vector<TcpServer *> servers{};
    // for (int i{1}; i < argc; i++)
    //     servers.push_back(new TcpServer{argv[i]});

    // MultiplexerIO &multiplexerio = MultiplexerIO::getInstance();

    for (auto &server : servers)
        addSocketToEpollFd(server, EPOLLIN);

    while (true)
    {
        int epollCount{epoll_wait(m_epollfd, m_events.data(), MAX_EVENTS, -1)};
        if (epollCount < 0)
        {
            std::perror("epoll_wait() failed");
            throw std::runtime_error("Error: epoll_wait() failed\n");
        }

        for (int i{0}; i < epollCount; i++) // Run through the existing connections looking for data to read
        {
            Socket *ePollDataPtr{static_cast<Socket *>(m_events.at(i).data.ptr)};

            if ((m_events.at(i).events & EPOLLIN) && ePollDataPtr->isReadReady) // If someone's ready to read
            {
                if (TcpServer *server = dynamic_cast<TcpServer *>(ePollDataPtr)) // If listener is ready to read, handle new connection
                {
                    Client *client = new Client{*server};
                    addSocketToEpollFd(client, EPOLLIN | EPOLLOUT | EPOLLRDHUP);
                    addSocketToEpollFd(client->timer, EPOLLIN);
                }
                else if (Timer *timer = dynamic_cast<Timer *>(ePollDataPtr))
                {
                    spdlog::warn("Timeout expired. Closing: {}", *(timer->m_client));
                    delete timer->m_client;
                }
                else if (Client *client = dynamic_cast<Client *>(ePollDataPtr)) // If not the listener, we're just a regular client
                {
                    handleConnectedClient(client);
                }
            }
            else if ((m_events.at(i).events & EPOLLOUT) && ePollDataPtr->isWriteReady) // If someone's ready to write
            {
                if (Client *client = dynamic_cast<Client *>(ePollDataPtr))
                {
                    HttpResponse response{client->httpRequest};
                    response.responseHandle();
                    spdlog::critical(response);

                    std::string s{response.responseBuild()};
                    spdlog::critical("s = |{}|", s);
                    send(client->m_socketFd, s.data(), s.length(), 0);
                    delete (client);
                }
            }
        }
    }
}

void MultiplexerIO::addSocketToEpollFd(Socket *ptr, int events)
{
    struct epoll_event ev
    {
    };
    ev.data.ptr = ptr;
    ev.events = events;

    if (epoll_ctl(m_epollfd, EPOLL_CTL_ADD, ptr->m_socketFd, &ev) == -1)
    {
        std::perror("epoll_ctl() failed");
        throw std::runtime_error("Error: epoll_ctl() failed\n");
    }
}

void MultiplexerIO::modifyEpollEvents(Socket *ptr, int events)
{
    struct epoll_event ev
    {
    };
    ev.data.ptr = ptr;
    ev.events = events;

    if (epoll_ctl(m_epollfd, EPOLL_CTL_MOD, ptr->m_socketFd, &ev) == -1)
        throw StatusCodeException(500, "Error: EPOLL_CTL_MOD failed");
}

// outstream operator overload
std::ostream &operator<<(std::ostream &out, const MultiplexerIO &multiplexerio)
{
    out << "MultiplexerIO(" << multiplexerio.m_epollfd << ")";

    return out;
}