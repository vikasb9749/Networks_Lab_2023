/*
    Name: Vikas Vijaykumar Bastewad
    Roll NO. 20CS10073
    Assignment No. 1
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

int main()
{
	int sockfd;
	struct sockaddr_in serv_addr;


	int flag = 1;
	while (flag)
	{
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

		char buf[100];
		char *buffer;
		size_t bufsize;
		size_t characters;

		printf("Enter the expression\n");
		characters = getline(&buffer, &bufsize, stdin);
		// printf("You entered: %s", buffer);

		if (buffer[0] == '-' && buffer[1] == '1' && buffer[2] == '\n')
		{
			flag = 0;
			close(sockfd);
			return 0;
		}
		
		send(sockfd, buffer, strlen(buffer) + 1, 0);
		recv(sockfd, buf, 100, 0);
		printf("%s\n", buf);
		close(sockfd);
	}

	return 0;
}
