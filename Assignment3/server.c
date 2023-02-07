/*
    Name: Vikas Vijaykumar Bastewad
    Roll NO. 20CS10073
    Assignment No. 3
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
	int server_port, sockfd, newsockfd; /* Socket descriptors */
	;
	struct sockaddr_in cli_addr, serv_addr;
	char buffer[200];
	int clilen;

	if (argc < 2)
	{
		printf("Usage: %s <server_port>\n", argv[0]);
		exit(1);
	}

	server_port = atoi(argv[1]);

	// Create a socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("Error creating socket");
		exit(1);
	}

	// Prepare the server address
	// memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(server_port);

	// Bind the socket to the server address
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("Error binding socket to server address");
		exit(0);
	}

	// Listen for incoming connections
	listen(sockfd, 5);
	printf("Server listening on port %d\n", server_port);
	srand(time(0));
	while (1)
	{
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

		if (newsockfd < 0)
		{
			perror("Accept error\n");
			exit(0);
		}
		// else
		// {
		// 	printf("Connection accepted \n");
		// }

		for (int i = 0; i < 200; i++)
		{
			buffer[i] = '\0';
		}

		if (recv(newsockfd, buffer, sizeof(buffer) + 1, 0) < 0)
		{
			perror("Error reading message from load balancer");
			close(newsockfd);
			continue;
		}

		// printf("Message received: %s", buffer);
		// Handle the incoming message
		if (strcmp(buffer, "Send Load") == 0)
		{
			int load = rand() % 100 + 1;
			printf("Load sent: %d\n", load);
			sprintf(buffer, "%d", load);
			if (send(newsockfd, buffer, sizeof(buffer) + 1, 0) < 0)
			{
				perror("Error writing load to client");
			}
		}
		else if (strcmp(buffer, "Send Time") == 0)
		{

			char buf[100];
			time_t t; // time_t represents the clock time as an integer which is a part of the calendar time.
			time(&t);
			strcpy(buf, ctime(&t));
			send(newsockfd, buf, strlen(buf) + 1, 0);
		}
		else
		{
			printf("Unknown message: %s\n", buffer);
			send(newsockfd, "Unknown message", 15, 0);
		}
		for (int i = 0; i < 200; i++)
			buffer[i] = '\0';
		recv(newsockfd, buffer, 200, 0);
		printf("%s\n", buffer);
		// Close the connection
		close(newsockfd);
	}

	return 0;
}
