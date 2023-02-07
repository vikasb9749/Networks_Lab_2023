/*
	Name: Vikas Vijaykumar Bastewad
	Roll NO. 20CS10073
	Assignment No. 1
	Problem-2
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* THE SERVER PROCESS */

float calc(char *buffer)
{

	int i = 0;
	float ans = 0;
	int flag = 1;
	while (buffer[i] != '\0')
	{
		char buf[1000];
		int j = 0;

		while (buffer[i] >= '0' && buffer[i] <= '9' || buffer[i] == '.' || buffer[i] == ' ')
		{

			// strcat(buf, buffer[i]);
			// if (buffer[i] == ' ')
			// {
			// 	i++;
			// 	continue;
			// }
			buf[j] = buffer[i];
			j++;
			i++;
		}

		float oprnd = atof(buf);
		char op;
		int t = i - j - 1;
		while (buffer[t] == ' ' && t >= 0)
		{
			t--;
		}
		op = buffer[t];
		// printf("oprnd=%f\n", oprnd);
		for (int k = 0; k < j; k++)
		{
			buf[k] = ' ';
		}
		if (ans == 0 && flag == 1)
		{
			ans = oprnd;
			flag = 0;
		}
		else
		{

			// printf("op=%c\n", op);
			if (op == '+')
			{
				ans += oprnd;
				// printf("ans=%f\n", ans);
			}
			else if (op == '-')
			{
				ans -= oprnd;
				// printf("ans=%f\n", ans);
			}
			else if (op == '*')
			{
				ans = ans * oprnd;

				// printf("ans=%f\n", ans);
			}
			else if (op == '/')
			{
				ans /= oprnd;
				// printf("ans=%f\n", ans);
			}
			else
			{
				if (op == '(')
				{
					float tmpAns = 0;
					char tmpBuf[1000];
					int m=0;
					while (buffer[i] == ')')
					{
						while (buffer[i] >= '0' && buffer[i] <= '9' || buffer[i] == '.' || buffer[i] == ' ')
						{

							// strcat(buf, buffer[i]);
							if (buffer[i] == ' ')
							{
								i++;
								continue;
							}
							tmpBuf[m] = buffer[i];
							m++;
							i++;
						}
					}
					i--;
					ans+= calc(tmpBuf);
					
				}
				// i++;
				// continue;
			}
		}
		i++;
	}
	// printf("Answer: %f", ans);
	return ans;
}
int main()
{
	int sockfd, newsockfd; /* Socket descriptors */
	int clilen;
	struct sockaddr_in cli_addr, serv_addr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Cannot create socket\n");
		exit(0);
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(20000);

	if (bind(sockfd, (struct sockaddr *)&serv_addr,
			 sizeof(serv_addr)) < 0)
	{
		perror("Unable to bind local address\n");
		exit(0);
	}

	listen(sockfd, 5);

	while (1)
	{

		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

		if (newsockfd < 0)
		{
			perror("Accept error\n");
			exit(0);
		}

		int i = 0;
		char buf[100000];

		recv(newsockfd, buf, 100000, 0);
		printf("%s\n", buf);
		if (buf[0] == '-' && buf[1] == '1' && buf[2] == '\n')
		{
			// flag = 0;
			close(newsockfd);
			// break;
			continue;
		}

		float ans = calc(buf);
		printf("Sending ans..\n");
		gcvt(ans, 6, buf); // convert float to string
		printf("ans=%s\n \n", buf);

		send(newsockfd, buf, strlen(buf) + 1, 0);

		close(newsockfd);
	}

	return 0;
}
