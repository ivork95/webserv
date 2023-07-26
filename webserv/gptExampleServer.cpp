#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFSIZE 1024
#define PORT 12345

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[BUFSIZE];

    // Create a TCP socket
    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    
    // Bind the socket to a specific IP address and port
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    
    // Listen for incoming connections
    listen(server_socket, 5);
    
    addr_size = sizeof(client_addr);
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);

    // Send and receive data
    strcpy(buffer, "Hello, Client!\n");
    send(client_socket, buffer, BUFSIZE, 0);

    close(client_socket);
    close(server_socket);

    return 0;
}
