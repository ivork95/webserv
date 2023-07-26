#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#define PORT 8080
  
int main(int argc, char const* argv[])
{
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    std::string hello = "Hello from client";
    char buffer[1024] = { 0 };
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
  
    if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0)
	{
       std::cerr << "\nConnection Failed \n" << std::endl;
        return -1;
    }
    send(client_fd, hello.c_str(), hello.length(), 0);
    std::cout << "Hello message sent" << std::endl;
    valread = read(client_fd, buffer, 1024);
    std::cout << buffer << std::endl;
  
    // closing the connected socket
    close(client_fd);
    return 0;
}