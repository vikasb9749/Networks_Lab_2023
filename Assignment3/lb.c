/*
	Name: Vikas Vijaykumar Bastewad
	Roll NO. 20CS10073
	Assignment No. 3
*/
// take port for load balancer as 2000 as i have used 2000 as the client

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

int server_ports[2];
int server_loads[2];

int main(int argc, char *argv[])
{
	int lb_port, sockfd, server_sockfd[2], newsockfd, servsock; // Socket descriptors
	struct sockaddr_in lb_addr, serv_lb_addr[2], cli_lb_addr;	// Socket addresses

	if (argc < 4) // Check for correct number of arguments
	{
		printf("Usage: %s <lb_port> <server1_port> <server2_port>\n", argv[0]);
		exit(0);
	}

	lb_port = atoi(argv[1]); // Get the load balancer port

	// Get the server ports
	server_ports[0] = atoi(argv[2]);
	server_ports[1] = atoi(argv[3]);

	// Create a socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) // Checking for errors
	{
		perror("Error creating socket");
		exit(0);
	}

	lb_addr.sin_family = AF_INET;		  // Load balancer address
	lb_addr.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY: The IP address is set to the address of the host
	lb_addr.sin_port = htons(lb_port);

	// servers
	serv_lb_addr[0].sin_family = AF_INET; // Server 1 address
	serv_lb_addr[0].sin_addr.s_addr = INADDR_ANY;
	serv_lb_addr[0].sin_port = htons(server_ports[0]);

	serv_lb_addr[1].sin_family = AF_INET; // Server 2 address
	serv_lb_addr[1].sin_addr.s_addr = INADDR_ANY;
	serv_lb_addr[1].sin_port = htons(server_ports[1]);

	// Bind the socket to the load balancer address
	if (bind(sockfd, (struct sockaddr *)&lb_addr, sizeof(lb_addr)) < 0)
	{
		perror("Error binding socket to load balancer address");
		exit(0);
	}

	// Listen for incoming connections
	listen(sockfd, 5);
	printf("Load balancer listening on port %d\n", lb_port);

	struct pollfd poll_set[1];
	poll_set[0].fd = sockfd;	 // fd: file descriptor
	poll_set[0].events = POLLIN; // POLLIN: Data other than high-priority data may be read without blocking
	poll_set[0].revents = 0;	 // revents: The revents field is set by the poll() system call to indicate the status of the file descriptor

	int val;	 // val: return value of poll()
	int cnt = 0; // cnt: count of the number of times the poll() is called

	while (1)
	{

		val = poll(poll_set, 1, 5000); // poll() returns the number of file descriptors with events or errors

		if (val < 0) // Checking for errors
		{
			perror("Error in poll");
			exit(0);
		}
		else if (val == 0) // poll timeout
		{

			for (int i = 0; i < 2; i++)
			{
				server_sockfd[i] = socket(AF_INET, SOCK_STREAM, 0); // Create a socket
				if (server_sockfd[i] < 0)
				{
					perror("Error creating socket");
					exit(0);
				}

				if (connect(server_sockfd[i], (struct sockaddr *)&serv_lb_addr[i], sizeof(serv_lb_addr[i])) < 0) // Connect to the server
				{
					perror("Error connecting to server\n");
					exit(0);
				}
				char buf_load[100]; // Buffer to store the load
				for (int j = 0; j < 100; j++)
					buf_load[j] = '\0';
				send(server_sockfd[i], "Send Load", 100, 0); // Send 'Send Load' to the server
				// printf("Sent 'Send Load' to server %d", i);
				recv(server_sockfd[i], buf_load, sizeof(buf_load) + 1, 0); // Receive the load from the server
				printf("Load received from server %d\n", i);			   // Print the load received from the server
				server_loads[i] = atoi(buf_load);						   // Convert the load to integer
				close(server_sockfd[i]);								   // Close the socket
			}
		}

		else // val > 0
		{

			int client_len = sizeof(cli_lb_addr);									  // Length of the client address
			newsockfd = accept(sockfd, (struct sockaddr *)&cli_lb_addr, &client_len); // Accept the incoming connection
			if (newsockfd < 0)
			{
				perror("Error accepting incoming connection\n");
				exit(0);
			}
			if (fork() == 0) // Create a child process
			{
				close(sockfd);
				char buf[100];

				// printf("server_loads[0] = %d, server_loads[1] = %d\n", server_loads[0], server_loads[1]);

				if (server_loads[0] < server_loads[1]) // selecting the server having less load
				{

					int servsock1 = socket(AF_INET, SOCK_STREAM, 0); // Create a socket
					if (connect(servsock1, (struct sockaddr *)&serv_lb_addr[0], sizeof(serv_lb_addr[0])) < 0)
					{
						perror("Error connecting to server");
						exit(0);
					}
					// printf("Connected to server 0\n");
					send(servsock1, "Send Time", sizeof("Send Time") + 1, 0); // Send 'Send Time' to the server
					for (int i = 0; i < 100; i++)
						buf[i] = '\0';
					recv(servsock1, buf, sizeof(buf) + 1, 0); // Receive the time from the server
					printf("\nSending client request to server 0\n");
					// printf("Received: %s", buf);
					send(newsockfd, buf, sizeof(buf) + 1, 0); // Send the time to the client
					close(servsock1);
					close(newsockfd);
					exit(0);
				}
				else
				{
					int servsock2 = socket(AF_INET, SOCK_STREAM, 0);
					if (connect(servsock2, (struct sockaddr *)&serv_lb_addr[1], sizeof(serv_lb_addr[1])) < 0)
					{
						perror("Error connecting to server");
						exit(0);
					}
					// printf("Connected to server 1");
					send(servsock2, "Send Time", sizeof("Send Time") + 1, 0);
					for (int i = 0; i < 100; i++)
						buf[i] = '\0';
					recv(servsock2, buf, sizeof(buf) + 1, 0);
					// printf("Received: %s", buf);
					printf("Sending client request to server 1\n");
					send(newsockfd, buf, sizeof(buf) + 1, 0);
					// send(newsockfd, buf, sizeof(buf) + 1, 0);
					close(servsock2);
					close(newsockfd);
					exit(0);
				}
			}
		}
	}

	// Close the load balancer socket
	close(sockfd);

	return 0;
}
