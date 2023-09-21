#include <csignal>
#include "TcpServer.hpp"
#include "ClientSocket.hpp"
#include "ServerIO.hpp"

#include "Lexer.hpp"
#include "Parser.hpp"
#include "Configuration.hpp"

int initConfig(const std::string &filePath);

void do_use_fd(int fd, ServerIO &serverio)
{
    char buf[256]; // Buffer for client data
    int nbytes = recv(fd, buf, sizeof(buf), 0);

    if (nbytes <= 0)
    {
        // Got error or connection closed by client
        if (nbytes == 0)
        {
            // Connection closed
            std::cout << "pollserver: socket " << fd << " hung up\n";
        }
        else
        {
            std::cerr << "Error: recv() failed\n";
        }

        serverio.deleteSocketFromEpollFd(fd);
        // close(pfds[i].fd); // Bye!
        // del_from_pfds(pfds, i, &fd_count);
    }
    else
    {
        // We got some good data from a client
        std::cout << "nbytes = " << nbytes << '\n';
        if (static_cast<unsigned int>(nbytes) < sizeof(buf))
            buf[nbytes] = '\0';
        else
            buf[sizeof(buf) - 1] = '\0';
        std::cout << buf;
    }
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
        throw std::runtime_error("Usage: ./webserv <config_file>\n\n\n");
	}
	// try {
	// 	int res = initConfig(argv[1]);
	// 	std::cout << res << std::endl;
	// } catch (std::exception &e) {
	// 	std::cout << e.what() << std::endl;
	// 	return 1;
	// }
	// return 0;

	if (initConfig(argv[1]))
		return 1;
	return 0;
	
    ServerIO serverio{};
    int epollCount;
    bool isLoopBroken{false};

    for (int i{1}; i < argc; i++)
    {
        std::unique_ptr<TcpServer> tcpserver = std::make_unique<TcpServer>(argv[i]);
        serverio.m_servers.push_back(std::move(tcpserver));
    }

    for (auto &server : serverio.m_servers)
        serverio.addSocketToEpollFd(server->m_serverSocket);

    while (true)
    {
        epollCount = epoll_wait(serverio.m_epollfd, serverio.m_events.data(), MAX_EVENTS, (3 * 60 * 1000));
        if (epollCount == -1)
        {
            std::perror("epoll_wait() failed");
            throw std::runtime_error("Error: epoll_wait() failed\n");
        }

        // Run through the existing connections looking for data to read
        for (int i{0}; i < epollCount; i++)
        {
            isLoopBroken = false;
            // Check if someone's ready to read
            if (serverio.m_events.at(i).events & EPOLLIN) // We got one!!
            {
                for (auto &server : serverio.m_servers)
                {
                    if (serverio.m_events.at(i).data.fd == server->m_serverSocket) // If listener is ready to read, handle new connection
                    {
                        std::unique_ptr<ClientSocket> clientsocket = std::make_unique<ClientSocket>(server->m_serverSocket);
                        // setnonblocking(conn_sock);
                        serverio.addSocketToEpollFd(clientsocket->m_clientSocket);
                        server->m_clientSockets.push_back(std::move(clientsocket));
                        isLoopBroken = true;
                        break;
                    }
                }
                if (!isLoopBroken) // If not the listener, we're just a regular client
                {
                    do_use_fd(serverio.m_events.at(i).data.fd, serverio);
                }
            }
        }
    }
}