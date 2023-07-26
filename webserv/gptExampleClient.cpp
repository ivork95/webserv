#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFSIZE 1024
#define PORT 12345

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFSIZE];

    // Create a TCP socket
    client_socket = socket(PF_INET, SOCK_STREAM, 0);
    
    // Specify server's IP address and port
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    // Send a connection request to the server
    connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    
    // Receive data from the server
    recv(client_socket, buffer, BUFSIZE, 0);
    printf("Received: %s", buffer);

    close(client_socket);

    return 0;
}
