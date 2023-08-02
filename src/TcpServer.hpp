#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#define BUFSIZE 1024
#define PORT 12345

#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>

class TcpServer
{
public:
    int m_serverSocket{};
    struct sockaddr_in m_serverAddr;

    // default constructor
    TcpServer(void);

    // destructor
    ~TcpServer(void);

};

// default constructor
TcpServer::TcpServer(void)
{
    std::cout << "TcpServer default constructor called\n";

    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_port = htons(PORT);
    m_serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    m_serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (m_serverSocket < 0)
    {
        std::perror("socket() failed");
        throw std::runtime_error("Error: socket() failed\n");
    }

    if (bind(m_serverSocket, (struct sockaddr *)&m_serverAddr, sizeof(m_serverAddr)) < 0)
    {
        std::perror("bind() failed");
        throw std::runtime_error("bind() failed");
    }

    if (listen(m_serverSocket, 5) < 0)
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