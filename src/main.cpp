#include "TcpServer.hpp"
#include "Client.hpp"

int main(void)
{
    TcpServer t;
    Client c;
    char buffer[BUFSIZE];
    ssize_t numBytes;

    c.m_clientSocket = accept(t.m_serverSocket, (struct sockaddr *)&c.m_clientAddr, &c.m_clientAddrSize);
    if (c.m_clientSocket == -1)
    {
        std::perror("accept() failed");
        throw std::runtime_error("Error: accept() failed\n");
    }

    numBytes = recv(c.m_clientSocket, buffer, BUFSIZE, 0);
    if (numBytes < 0)
    {
        std::perror("recv() failed");
    }
    else if (numBytes == 0)
    {
        std::cout << "Client disconnected before sending data.\n";
    }
    else
    {
        // Null-terminate the received data in case it's a string
        if (numBytes < BUFSIZE)
            buffer[numBytes] = '\0';
        else
            buffer[BUFSIZE - 1] = '\0';

        // Print the received data
        std::cout << "Received from client: " << buffer << "\n";

        // Echo the data back to the client
        if (send(c.m_clientSocket, buffer, numBytes, 0) != numBytes)
            std::perror("send() failed to send all data");
    }
}
