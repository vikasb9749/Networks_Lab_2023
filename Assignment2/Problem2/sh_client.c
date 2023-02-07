/*
    Name: Vikas Vijaykumar Bastewad
    Roll NO. 20CS10073
    Assignment No. 2
    Problem-2
*/
/*    THE CLIENT PROCESS */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX 50		// Maximum size of the buffer
#define MxCmd 200	// Maximum size of the command
int main()
{
	int sockfd;
	struct sockaddr_in serv_addr;

	int i;
	char buf[MAX];		// Buffer to store the data

	/* Opening a socket is exactly similar to the server process */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Unable to create socket\n");
		exit(0);
	}

	serv_addr.sin_family = AF_INET;
	inet_aton("127.0.0.1", &serv_addr.sin_addr);
	serv_addr.sin_port = htons(20000);

	if ((connect(sockfd, (struct sockaddr *)&serv_addr,
				 sizeof(serv_addr))) < 0)
	{
		perror("Unable to connect to server\n");
		exit(0);
	}

	for (i = 0; i < MAX; i++)
		buf[i] = '\0';
	recv(sockfd, buf, MAX, 0);		// Receiving the LOGIN message from the server
	printf("%s\n", buf);   			// Printing the LOGIN message

	printf("Enter the username: \n"); 	// Taking the username from the user
	scanf("%s", buf);
	char c = getchar();					// To remove the newline character

	send(sockfd, buf, strlen(buf) + 1, 0);	// Sending the username to the server

	char findAns[MAX];			// Buffer to store the response from the server
	for (i = 0; i < MAX; i++)
		findAns[i] = '\0';
	recv(sockfd, findAns, MAX, 0);	// Receiving the response from the server
	printf("%s\n", findAns);		
	if (strcmp(findAns, "FOUND") == 0)	// If the username is found
	{
		printf("Enter the command: \n");	// Taking the command from the user

		char cmd[MxCmd];	// Buffer to store the command
		
	
		fgets(cmd, 200, stdin);		// Taking the command from the user
		cmd[strlen(cmd) - 1] = '\0';	// Removing the newline character
		

		while (strcmp(cmd, "exit") != 0 )		// Loop to run the commands
		{

			// sending the command in chunks of 50 bytes
			char name[50];		
			int i = 0;
			int j = 0;
			while (cmd[j] != '\0')
			{
				name[i] = cmd[j];
				i++;
				j++;
				if (i == 50)
				{
					send(sockfd, name, strlen(name), 0);
					i = 0;
					for (int k = 0; k < 50; k++)
					{
						name[k] = '\0';
					}
				}
			}
			name[i] = '\0';
			send(sockfd, name, strlen(name), 0);	

			// receiving the output in chunks of 50 bytes
			for (i = 0; i < MAX; i++)
				buf[i] = '\0';
			recv(sockfd, buf, MAX, 0);

			if (strcmp(buf, "####") == 0)		
			{
				printf("Error in running command\n");
			}
			else if (strcmp(buf, "$$$$") == 0)  
			{
				printf("Invalid  command\n");
			}
			else				
			{
				printf("%s", buf);
			}


			while (buf[MAX - 1] != '\0')
			{
				for (i = 0; i < MAX; i++)
					buf[i] = '\0';
				recv(sockfd, buf, MAX, 0);
				printf("%s", buf);
			}
			printf("\nEnter the command: \n");
			

			fgets(cmd, 200, stdin);
			cmd[strlen(cmd) - 1] = '\0';

			
		}
	}
	else		// If the username is not found
	{
		printf("Invalid username\n");
		close(sockfd);
	}

	close(sockfd);
	return 0;
}
