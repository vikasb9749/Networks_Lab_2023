/*
    Assignment No. 4
    Group No. 19
    Vikas Vijaykumar Bastewad  (20CS10073)
    Yashraj Singh  (20CS10079)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define BUFFERSIZE 1024

int main()
{
    int sockfd, newsockfd; /* Socket descriptors */
    int clilen;
    struct sockaddr_in cli_addr, serv_addr;

    int i;
    char buf[BUFFERSIZE]; /* We will use this buffer for communication */

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Cannot create socket\n");
        exit(0);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(8080);

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

            close(sockfd);

            for (i = 0; i < BUFFERSIZE; i++)
                buf[i] = '\0';
            recv(newsockfd, buf, BUFFERSIZE, 0);
            printf("*****************Request*****************\n");
            printf("%s\n", buf);

            //<Date(ddmmyy)>:<Time(hhmmss)>:<Client IP>:<Client Port>:<GET/PUT>:<URL> 
            char AccessLog[1000];
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            strcpy(AccessLog, "");
            strcat(AccessLog, "AccessLog.txt");
            FILE *fp = fopen(AccessLog, "a");
            fprintf(fp, "%02d%02d%02d: %02d%02d%02d: %s : %d :", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
            

            // now we have to parse the request
            char *token = strtok(buf, " ");
            char *cmd = token;
            // printf("cmd = %s \n", cmd);
            fprintf(fp, "%s :", cmd);
            fprintf(fp, "http://");
            fprintf(fp, "%s", inet_ntoa(serv_addr.sin_addr));
            fprintf(fp, "%d\n", ntohs(serv_addr.sin_port));
            fclose(fp);

            token = strtok(NULL, " ");
            char *path = token;
            // printf("path = %s \n", path);

            token = strtok(NULL, " ");
            char *http = token;
            // printf("http = %s \n", http);

            token = strtok(NULL, " ");
            token = strtok(NULL, " ");
            char *host = token;
            // printf("host = %s \n", host);

            token = strtok(NULL, " ");
            token = strtok(NULL, " ");
            char *connection = token;
            // printf("connection = %s \n", connection);

            token = strtok(NULL, " ");
            token = strtok(NULL, "A");

            char *date = token;
            // printf("date = %s", date);

            token = strtok(NULL, " ");
            token = strtok(NULL, " ");
            char *accept = token;
            // printf("accept = %s \n", accept);

            token = strtok(NULL, " ");
            token = strtok(NULL, " ");
            char *accept_language = token;
            // printf("accept_language = %s \n", accept_language);

            token = strtok(NULL, " ");
            token = strtok(NULL, "C");
            char *if_modified_since = token;
            // printf("if_modified_since = %s", if_modified_since);

            token = strtok(NULL, " ");
            token = strtok(NULL, " ");
            char *content_language = token;
            // printf("content_language = %s \n", content_language);

            char *content_length = malloc(50);
            int len = 0;

            if (strcmp(cmd, "PUT") == 0)
            {
                token = strtok(NULL, " ");
                token = strtok(NULL, "\n");
                content_length = token;
                // printf("content len=%s\n", content_length);
            }

            token = strtok(NULL, " ");
            token = strtok(NULL, " ");
            char *content_type = token;
            // printf("contentType=%s\n", content_type);

            // handeling the GET command
            if (strcmp(cmd, "GET") == 0)
            {
                   // printf("GET request\n");
                // now we have to send the response
                char response[BUFFERSIZE];
                for (i = 0; i < BUFFERSIZE; i++)
                    response[i] = '\0';

                time_t t;
                time(&t);
                // char *time = ctime(&t);
                t += 3 * 24 * 60 * 60; // expires = current time + 3 days
                char *expires = ctime(&t);

                strcat(response, "HTTP/1.1 200 OK");
                // strcat(response, status);
                strcat(response, " \n");

                strcat(response, "Expires: ");
                strcat(response, expires);

                strcat(response, "Cache-Control: no-store always \n");
                strcat(response, "Content-Language: en-US \n");

                strcat(response, "Content-Type: ");
                strcat(response, content_type);
                strcat(response, " \n");

                strcat(response, "Last-Modified: ");
                t -= 3 * 24 * 60 * 60; // last modified = current time
                strcat(response, ctime(&t));
                strcat(response, " \n");

                // printf("response = %s \n", response);

                send(newsockfd, response, strlen(response) + 1, 0);

                // open the file and send the content to the client
                // printf("GET request\n");
                char filename[100];
                strcpy(filename, path);
                // printf("filename = %s \n", filename);

                int length = strlen(filename);
                for (i = 0; i < length - 1; i++)
                {
                    filename[i] = filename[i + 1];
                }
                filename[i] = '\0';
                // printf("filename = %s \n", filename);
                
                FILE *fp = fopen(filename, "r");

                // fseek(fp, 0, SEEK_SET);
                if (fp != NULL)
                {
                    char content[100];
                    int content_size;
                    while ((content_size = fread(content, 1, 100, fp)) > 0)
                    {
                        int n = send(newsockfd, content, content_size, 0);
                        // printf("n=%d\n",n);
                        // printf("content = %s \n", content);
                        for (int i = 0; i < 100; i++)
                            content[i] = '\0';
                    }
                }

            }

            // handeling the PUT command
            if (strcmp(cmd, "PUT") == 0)
            {
                // printf("in the put command\n");
                // now we have to send the response
                char response[BUFFERSIZE];
                for (i = 0; i < BUFFERSIZE; i++)
                    response[i] = '\0';

                time_t t;
                time(&t);
                // char *time = ctime(&t);
                t += 3 * 24 * 60 * 60; // expires = current time + 3 days
                char *expires = ctime(&t);

                strcat(response, "HTTP/1.1 200 OK");
                // strcat(response, status);
                strcat(response, " \n");

                strcat(response, "Expires: ");
                strcat(response, expires);

                strcat(response, "Cache-Control: no-store always \n");
                strcat(response, "Content-Language: en-US \n");

                strcat(response, "Content-Type: ");
                strcat(response, content_type);
                strcat(response, " \n");

                strcat(response, "Last-Modified: ");
                t -= 3 * 24 * 60 * 60; // last modified = current time
                strcat(response, ctime(&t));
                strcat(response, " \n");

                // sleep(4);    // this is for checking the timeout
                
                send(newsockfd, response, strlen(response) + 1, 0);

                // open the file and send the content to the client
                // printf("GET request\n");
                char filename[100];
                strcpy(filename, path);
                // printf("filename = %s \n", filename);

                int length = strlen(filename);
                for (i = 0; i < length - 1; i++)
                {
                    filename[i] = filename[i + 1];
                }
                filename[i] = '\0';
                // printf("filename = %s \n", filename);

                char *token5 = strtok(filename, "/");
                char *fileFormat;
                while (token5 != NULL)
                {
                    // printf("%s\n", token5);
                    fileFormat = token5;
                    token5 = strtok(NULL, "/");
                }

                char entity_body[100];
                for (i = 0; i < 100; i++)
                    entity_body[i] = '\0';

                FILE *fp = fopen(fileFormat, "w");
                if (fp != NULL)
                {
                    int content_size;
                    printf("Downloading file...\n");
                    while ((content_size = recv(newsockfd, entity_body, 100, 0)) > 0)
                    {
                        int n = fwrite(entity_body, 1, content_size, fp);
                        // printf("n = %d\n", n);
                        // printf("entity_body = %s\n", entity_body);
                        for (i = 0; i < 100; i++)
                            entity_body[i] = '\0';
                    }
                    printf("File downloaded successfully\n");
                }
            }


            close(newsockfd);
            exit(0);
        }

        close(newsockfd);
    }
    return 0;
}

