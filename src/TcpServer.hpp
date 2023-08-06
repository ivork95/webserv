#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#define BUFSIZE 1024

#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <vector>
#include "ClientSocket.hpp"
#include <memory>

class TcpServer
{
public:
    int m_serverSocket{};
    struct sockaddr_in m_serverAddr
    {
    };
    std::vector<std::unique_ptr<ClientSocket>> m_clientSockets{};

    // default constructor
    TcpServer(void) = delete;

    // port constructor
    TcpServer(const unsigned int port);

    // destructor
    ~TcpServer(void);
};

// port constructor
TcpServer::TcpServer(const unsigned int port)
{
    std::cout << "TcpServer port constructor called\n";

    int on{1};

    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_port = htons(port);
    m_serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    m_serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (m_serverSocket < 0)
    {
        std::perror("socket() failed");
        throw std::runtime_error("Error: socket() failed\n");
    }

    if (setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        std::perror("setsockopt() failed");
        throw std::runtime_error("setsockopt() failed");
    }

    if (bind(m_serverSocket, (struct sockaddr *)&m_serverAddr, sizeof(m_serverAddr)) < 0)
    {
        std::perror("bind() failed");
        throw std::runtime_error("bind() failed");
    }

    if (listen(m_serverSocket, 32) < 0)
    {
        std::perror("listen() failed");
        throw std::runtime_error("Error: listen() failed\n");
    }

    std::cout << "TcpServer listening...\n";
}

// destructor
TcpServer::~TcpServer(void)
{
    std::cout << "TcpServer destructor called\n";

    close(m_serverSocket);
}

#endif
