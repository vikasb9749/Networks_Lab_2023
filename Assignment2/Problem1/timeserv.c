/*
    Name: Vikas Vijaykumar Bastewad
    Roll NO. 20CS10073
    Assignment No. 2
    Problem-1
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#define MAXLINE 1024 // maximum length of the buffer

int main()
{
    int sockfd;             // sockfd: socket file descriptor
    struct sockaddr_in servaddr, cliaddr;   // servaddr: server address, cliaddr: client address
    while (1)       
    {
        // Create socket file descriptor
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0)
        {
            perror("socket creation failed");
            exit(EXIT_FAILURE);
        }

        memset(&servaddr, 0, sizeof(servaddr)); 
        memset(&cliaddr, 0, sizeof(cliaddr));

        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = INADDR_ANY;
        servaddr.sin_port = htons(8181);

        // Bind the socket with the server address
        if (bind(sockfd, (const struct sockaddr *)&servaddr,
                 sizeof(servaddr)) < 0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }

        printf("\nServer Running....\n");

        int n;
        socklen_t len;
        char buffer[MAXLINE];

        len = sizeof(cliaddr);
        n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0,
                     (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';
        printf("%s\n", buffer);

        char buf[100];
        time_t t;               // t: time variable
        time(&t);               // time(&t) returns the current time of the system
        strcpy(buf, ctime(&t)); // ctime(&t) returns the current time in string format

        // This send call is without any sleep time i.e. sleep time = 0 client will get the time instantly
        //(I have written this to crosscheck the  difference between this time and time after the sleep which the client will receive)
        // comment out the following code to check the difference
        // sendto(sockfd, (const char *)buf, strlen(buf), 0,
        //        (const struct sockaddr *)&cliaddr, sizeof(cliaddr));
        // printf("Time sent from server to client\n");

        // Sleep time = 8 -> In this case client will receive the time as client is trying for 5 times, 3 sec each i.e. sleep time < 15
        sleep(8);               // sleep for 8 sec
        time(&t);               // get the current time
        strcpy(buf, ctime(&t)); // convert the time to string format
        sendto(sockfd, (const char *)buf, strlen(buf), 0,
               (const struct sockaddr *)&cliaddr, sizeof(cliaddr)); // send the time to the client

        // Sleep time = 16 -> client will not receive the time as client sleep time > 15, results in timeout exceeded
        //  comment out the following lines to check this
        //  sleep(16);
        //  time(&t);
        //  strcpy(buf, ctime(&t));
        //  sendto(sockfd, (const char *)buf, strlen(buf), 0,
        //         (const struct sockaddr *)&cliaddr, sizeof(cliaddr));

        close(sockfd); // close the socket
    }
    return 0;
}