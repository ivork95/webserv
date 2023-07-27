#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <cstring>

const int PORT = 9000;
const int MAX_EVENTS = 10; // The maximum number of events to be returned from epoll_wait()
const int BUFSIZE = 1024;

void handleClient(int client_socket) {
	char buffer[BUFSIZE];
	std::string hello = "Hello from server";

	int valread = read(client_socket, buffer, 1024);
	std::cout << buffer << std::endl;

	send(client_socket, hello.c_str(), hello.length(), 0);
	std::cout << "Hello message sent" << std::endl;

	close(client_socket);
}

int main() 
{
	int socket_fd, client_socket;
	int opt = 1;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
		std::cerr << "Failed to create socket" << std::endl;
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (bind(socket_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		std::cerr << "Bind failure" << std::endl;
		exit(EXIT_FAILURE);
	}

    if (listen(socket_fd, 3) < 0) {
        std::cerr << "Listen error" << std::endl;
        exit(EXIT_FAILURE);
    }

	// Create epoll instance
	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
	{
		std::cerr << "Failed to create epoll instance" << std::endl;
		exit(EXIT_FAILURE);
	}

	struct epoll_event event, events[MAX_EVENTS];
	event.events = EPOLLIN;
	event.data.fd = socket_fd;

	// Add the listening socket to epoll instance
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &event) == -1)
	{
		std::cerr << "Failed to add socket to epoll" << std::endl;
		exit(EXIT_FAILURE);
	}

	while (true)
	{
		int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (num_events == -1) {
			std::cerr << "epoll_wait error" << std::endl;
			exit(EXIT_FAILURE);
		}

		for (int i = 0; i < num_events; ++i)
		{
			if (events[i].data.fd == socket_fd)
			{
				// New client connection is available
				client_socket = accept(socket_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
				if (client_socket == -1) {
					std::cerr << "Accept error" << std::endl;
					exit(EXIT_FAILURE);
				}

				event.events = EPOLLIN;
				event.data.fd = client_socket;

				// Add the new client socket to epoll instance
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &event) == -1)
				{
					std::cerr << "Failed to add client socket to epoll" << std::endl;
					exit(EXIT_FAILURE);
				}
			}
			else
			{
				// Data is available to read on an existing client socket
				handleClient(events[i].data.fd);

				// Remove the client socket from epoll instance after handling it
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
			}
		}
	}

	// Close the listening socket and epoll instance
	close(socket_fd);
	close(epoll_fd);

	return 0;
}
