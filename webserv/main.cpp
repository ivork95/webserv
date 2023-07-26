#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

int main()
{
	int socket_fd, new_socket, valread;
	int opt = 1;
	struct sockaddr_in address; //custom data structure
	int addrlen = sizeof(address);
	const int PORT = 8080;
    char buffer[1024] = { 0 };
    std::string hello = "Hello from server";

	if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) // create socket with IPv4 address family with TCP(SOCK_STREAM)
	{
		std::cerr << "Failed to create socket" << std::endl;
		exit(EXIT_FAILURE);
	}

	// if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) != 0)
	// {
	// 	std::cerr << "setsockopt error" << std::endl;
	// 	exit(EXIT_FAILURE);
	// }
	address.sin_family = AF_INET; // IPv4 Internet protocols
	address.sin_addr.s_addr = INADDR_ANY; // bind to any IP address
	address.sin_port = htons(PORT); // bind to port number 8080

    if (bind(socket_fd, (struct sockaddr*)&address, sizeof(address)) < 0) //assigning a transport address to the socket
	{
        std::cerr << "bind failure" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (listen(socket_fd, 3) < 0) // waits for the client to make a connection with server
	{
        std::cerr << "listen error" << std::endl;
        exit(EXIT_FAILURE);
	}
    if ((new_socket = accept(socket_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0)
	{
        std::cerr << "accept" << std::endl;
        exit(EXIT_FAILURE);
    }
    valread = read(new_socket, buffer, 1024);
	std::cout << buffer << std::endl;
    send(new_socket, hello.c_str(), hello.length(), 0);
    std::cout << "Hello message sent\n" << std::endl;
  
    // closing the connected socket
    close(new_socket);
    // closing the listening socket
    shutdown(socket_fd, SHUT_RDWR);
}