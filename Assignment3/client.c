/*
    Name: Vikas Vijaykumar Bastewad
    Roll NO. 20CS10073
    Assignment No. 3
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h> // header file contains definitions of functions to get and manipulate date and time information.

/* THE CLIENT PROCESS */

int main()
{
    int newsockfd; /* Socket descriptors */
    struct sockaddr_in serv_addr;
   
    if ((newsockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Cannot create socket\n");
        exit(0);
    }

    serv_addr.sin_family = AF_INET;
    // inet_aton("127.0.0.1", &serv_addr.sin_addr);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(2000);

    // connect the client socket to server socket
    if (connect(newsockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("connection with the server failed...\n");
        exit(0);
    }
    // else
    //     printf("connected to the server.\n");


    char buf[100];  /* buffer for communication */
    // time_t t;
    // time(&t);
    
    recv(newsockfd, buf, 100, 0);
    printf("%s\n", buf);

    close(newsockfd);
}