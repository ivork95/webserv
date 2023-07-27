#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

#define PORT 9000
#define BUFSIZE 1024

int main(int argc, char const* argv[])
{
	int client_fd;
	struct sockaddr_in serv_addr;
	char buffer[BUFSIZE];

	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "Socket creation error" << std::endl;
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
	{
		std::cerr << "Invalid address/ Address not supported" << std::endl;
		return -1;
	}

	if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
	{
		std::cerr << "\nConnection Failed \n" << std::endl;
		return -1;
	}

	std::string hello = "Hello from client.";
	send(client_fd, hello.c_str(), hello.length(), 0);
	std::cout << "Hello message sent" << std::endl;
	recv(client_fd, buffer, BUFSIZE, 0);
	std::cout << buffer << std::endl;
  
	// closing the connected socket
	close(client_fd);
	return 0;
}
