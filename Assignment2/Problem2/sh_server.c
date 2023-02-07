/*
    Name: Vikas Vijaykumar Bastewad
    Roll NO. 20CS10073
    Assignment No. 2
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
#include <dirent.h>
#define MAX 50

/* THE SERVER PROCESS */

void presentWorkingDirectory(int newsockfd)
{
	char wd[255];
	wd[255 - 1] = '\0';
	

	if (getcwd(wd, 255 - 1) == NULL)
	{
		// printf("Can not get current working directory\n");
		char *err = "####";
		send(newsockfd, err, strlen(err) + 1, 0);
	}
	else
	{
		// sending the result to the client process in chunks of 50 bytes
		char name[50];
		int i = 0;
		int j = 0;
		while (wd[j] != '\0')
		{
			name[i] = wd[j];
			i++;
			j++;
			if (i == 50)
			{
				send(newsockfd, name, strlen(name), 0);
				i = 0;
				for (int k = 0; k < 50; k++)
				{
					name[k] = '\0';
				}
			}
		}
		name[i] = '\0';
		send(newsockfd, name, strlen(name) + 1, 0);
	}
}

void listDirectory(int newsockfd, char *path)
{

	struct dirent *de; // Pointer for directory entry

	// opendir() returns a pointer of DIR type.
	if (path == " ")
	{
		path = ".";
	}
	DIR *dr = opendir(path);

	if (dr == NULL) // opendir returns NULL if couldn't open directory
	{
		printf("Could not open current directory");
		char *err = "####";
		send(newsockfd, err, strlen(err) + 1, 0);
	}
	char list[1000];
	list[1000 - 1] = '\0';
	while ((de = readdir(dr)) != NULL)
	{

		char *name = de->d_name;
		strcat(list, name);
		strcat(list, "\n");
	}
	// sending the result to the client process in chunks of 50 bytes
	char name[50];
	int i = 0;
	int j = 0;
	while (list[j] != '\0')
	{
		name[i] = list[j];
		i++;
		j++;
		if (i == 50)
		{
			send(newsockfd, name, strlen(name), 0);
			i = 0;
			for (int k = 0; k < 50; k++)
			{
				name[k] = '\0';
			}
		}
	}
	name[i] = '\0';
	send(newsockfd, name, strlen(name), 0);

	// closedir(dr);
}



int main()
{
	int sockfd, newsockfd; /* Socket descriptors */
	int clilen;
	struct sockaddr_in cli_addr, serv_addr;

	int i;
	char buf[50]; /*  buffer for communication */

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Cannot create socket\n");
		exit(0);
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(20000);

	if (bind(sockfd, (struct sockaddr *)&serv_addr,
			 sizeof(serv_addr)) < 0)
	{
		printf("Unable to bind local address\n");
		exit(0);
	}

	listen(sockfd, 5);

	while (1)
	{

		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr,
						   &clilen);

		if (newsockfd < 0)
		{
			printf("Accept error\n");
			exit(0);
		}

		if (fork() == 0)
		{

			/* This child process will now communicate with the
			   client through the send() and recv() system calls.
			*/
			close(sockfd); /* Close the old socket since all
					  communications will be through
					  the new socket.
				   */

			strcpy(buf, "LOGIN:");		
			send(newsockfd, buf, strlen(buf) + 1, 0);	// send login message

			for (i = 0; i < 50; i++)
				buf[i] = '\0';
			recv(newsockfd, buf, 50, 0);		// receive username
			printf("%s\n", buf);
			char username[25];		
			strcpy(username, buf);

			// Declare the file pointer
			FILE *filePointer;

			// Declare the variable for the data to be read from file
			char readData[25];
			filePointer = fopen("users.txt", "r");

			int flag = 0; // flag to check if user exists

			// Check if this filePointer is null
			// which maybe if the file does not exist
			if (filePointer == NULL)
			{
				printf(" file failed to open.");
			}
			else
			{

				int res = 1;	// to check if username is found

				while (fgets(readData, 25, filePointer) != NULL) 	// read line by line
				{

					int l = strlen(readData);
					readData[l - 1] = '\0';
					res = strcmp(readData, username);
					if (res == 0)
					{

						flag = 1;

						break;
					}
				}

				fclose(filePointer);		// close the file
				if (flag == 0)			
				{
					printf("User not found\n");
					strcpy(buf, "NOT-FOUND");
				}
				else
				{
					printf("User found\n");
					strcpy(buf, "FOUND");
				}
				send(newsockfd, buf, strlen(buf) + 1, 0);		// send the result to client

				char command[50];		// to store the command buffer of chunk size 50
				char finalCmd[200] = "";	// to store the final command
				while (1)
				{
					// receiving the command from client in chunks of 50 bytes and storing it in finalCmd
					for (i = 0; i < MAX; i++)
						command[i] = '\0';
					recv(newsockfd, command, MAX, 0);		
					strcpy(finalCmd, command);
					
					while (command[MAX - 1] != '\0')
					{
						for (i = 0; i < MAX; i++)
							command[i] = '\0';
						recv(sockfd, command, MAX, 0);
						strcat(finalCmd, command);
						
					}
					

					// printf("%s\n", finalCmd);

					// separating the command and the arguments
					char A[200];	
					for (i = 0; finalCmd[i] != '\0'; i++)
					{
						if (finalCmd[i] != ' ')
						{
							A[i] = finalCmd[i];
						}
						else
						{
							A[i] = '\0';
							break;
						}
					}
					// printf("%s\n", A);
					
					char B[200] = "";
					if (strlen(A) != strlen(finalCmd)) // if there are arguments
					{

						int j = 0;
						for (i = i + 1; finalCmd[i] != '\0'; i++)
						{
							
							if (finalCmd[i] != ' ')
							{
								B[j] = finalCmd[i];
							}
							else
							{
								B[j] = '\0';
								break;
							}
							j++;
						}
						B[j] = '\0';
						// printf("%s\n", B);
					}

					
					// checking the command and performing the required operation
					if (strcmp(A, "pwd") == 0)
					{
						
						presentWorkingDirectory(newsockfd);
					}
					else if (strcmp(A, "dir") == 0)
					{
						if (strcmp(B, "") != 0) // if there are arguments
						{
							listDirectory(newsockfd, B);
						}
						else
						{
							listDirectory(newsockfd, " ");
						}
					}
					else if (strcmp(A, "cd") == 0)
					{
						
						if (chdir(B) < 0)  // if there is an error in changing the directory
						{
							char err[5];
							strcpy(err, "####");
							send(newsockfd, err, strlen(err) + 1, 0);
						}
						else
						{
							send(newsockfd, "Directory changed!!", strlen("Directory changed!!") + 1, 0);
						}
					}

					else
					{
						// printf("Command not found\n");
						char inv[5];
						strcpy(inv, "$$$$");
						send(newsockfd, inv, strlen(inv) + 1, 0);
						
					}
				}
			}
			close(newsockfd);
			exit(0);
		}

		close(newsockfd);
	}
	return 0;
}
