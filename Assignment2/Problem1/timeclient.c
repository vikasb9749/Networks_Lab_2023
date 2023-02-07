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
#include <poll.h>
#include <time.h>
int main()
{
    int sockfd;
    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8181);
    inet_aton("127.0.0.1", &servaddr.sin_addr);

    int n;
    socklen_t len;

    char *hello = "CLIENT:HELLO";
    // connecting to the server
    sendto(sockfd, (const char *)hello, strlen(hello), 0,
           (const struct sockaddr *)&servaddr, sizeof(servaddr));

    // the below commented code is written for checking purpose only
    //  char buffer[100];
    //  len = sizeof(servaddr);
    //  n = recvfrom(sockfd, (char *)buffer, 100, 0,
    //               (struct sockaddr *)&servaddr, &len);
    //  buffer[n] = '\0';
    //  // printf("Sleep Time = 0 sec :->");
    //  printf("%s\n", buffer);

    struct pollfd poll_set[1];
    poll_set[0].fd = sockfd;     // fd: file descriptor
    poll_set[0].events = POLLIN; // POLLIN: Data other than high-priority data may be read without blocking
    poll_set[0].revents = 0;     // revents: The revents field is set by the poll() system call to indicate the status of the file descriptor

    int val;                                      // val: return value of poll()
    int cnt = 0;                                  // cnt: count of the number of times the poll() is called
    while ((val = poll(poll_set, 1, 3000)) != -1) // poll() is called for every 3 sec
    {
        if (poll_set[0].revents) // if the revents is not 0 then the data is received
        {
            /* Priority data may be written on device number 1 */
            char buffer[100];
            len = sizeof(servaddr);
            n = recvfrom(sockfd, (char *)buffer, 100, 0,
                         (struct sockaddr *)&servaddr, &len);
            buffer[n] = '\0';
            // printf("Sleep Time > 0 sec :-> \t");
            printf("%s\n", buffer);
            exit(0);
        }
        // the data is not received in the waiting time of 3 sec so incrementing the count and checking for next 3 sec
        if (val == 0)
        {
            cnt++;
            printf("try %d\n", cnt);
            if (cnt == 5) // if the count is more than 5 then Timeout exceeded will be printed and then exit.
            {
                printf("Timeout exceeded\n"); // Timeout exceeded
                exit(0);                      // exit
            }
        }
    }
    close(sockfd); // closing the socket
    return 0;
}