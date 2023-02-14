/*
    Assignment No. 4
    Group No. 19
    Vikas Vijaykumar Bastewad  (20CS10073)
    Yashraj Singh  (20CS10079)
*/

// HTTP 1.1 Client

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <poll.h>

#define MAXLEN 100
#define BUFFERSIZE 1024

char *giveDate(char *dateFor, time_t t)
{
    // printf("Current date and time is: %s", ctime(&t));
    char *token4 = ctime(&t);
    char *day = strtok(token4, " ");
    char *month = strtok(NULL, " ");
    char *date = strtok(NULL, " ");
    char *time = strtok(NULL, " ");
    char *year = strtok(NULL, " ");

    char dateFormat[BUFFERSIZE];

    strcpy(dateFormat, day);
    // printf("dateFormat = %s\n", dateFormat);
    strcat(dateFormat, ", ");
    strcat(dateFormat, date);
    strcat(dateFormat, " ");
    strcat(dateFormat, month);
    strcat(dateFormat, " ");
    strcat(dateFormat, year);
    strcat(dateFormat, " ");
    strcat(dateFormat, time);
    strcat(dateFormat, " GMT");

    dateFor = dateFormat;
    return dateFor;
}

char *chooseFileFormat(char *fileformat)
{
    if (strcmp(fileformat, "html") == 0)
    {
        fileformat = "text/html";
    }
    else if (strcmp(fileformat, "pdf") == 0)
    {
        fileformat = "application/pdf";
    }
    else if (strcmp(fileformat, "jpg") == 0)
    {
        fileformat = "image/jpeg";
    }
    else
    {
        fileformat = "text/*";
    }
    return fileformat;
}

int main()
{
    int sockfd;
    struct sockaddr_in serv_addr;

    while (1)
    {

        /* Opening a socket is exactly similar to the server process */
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("Unable to create socket\n");
            exit(0);
        }
        // prompt for command.
        char command[BUFFERSIZE];
        printf("MyOwnBrowser> ");
        // input the command
        fgets(command, MAXLEN, stdin);

        // printf("%s\n", command);

        // command format: GET {URL}
        // GET http://192.168.50.78/docs/a1.pdf:8080
        // PUT http://192.168.50.78/docs/a1.pdf:8080 abc.txt
        // GET http://127.0.0.1/docs/abc.txt:8080
        // PUT http://127.0.0.1/docs:8080 abc.txt

        // Returns first token
        char *token = strtok(command, " ");
        char *cmd = token;
        char c = cmd[strlen(cmd) - 1];
        cmd[strlen(cmd) - 1] = '\0';
        if (strcmp(cmd, "QUIT") == 0)
        {
            printf("Bye!\n");
            exit(0);
        }

        cmd[strlen(cmd)] = c;
        token = strtok(NULL, " ");
        char *url = token;
        // printf("cmd = %s, url = %s \n", cmd, url);

        char cLength[BUFFERSIZE];
        if (strcmp(cmd, "PUT") == 0)
        {
            token = strtok(NULL, " ");
            char *filename = token;
            // filename[strlen(filename) - 1] = '\0';
            // printf("filename = %s, length=%d\n", filename, strlen(filename));

            // Returns first token
            char *token2 = strtok(url, "://");
            char *protocol = token2;
            // printf("protocol = %s\n", protocol);

            token2 = strtok(NULL, "/");
            char *host = token2;
            // printf("host = %s\n", host);

            token2 = strtok(NULL, ":");
            char *path = token2;
            char *tempPath = (char *)malloc(strlen(path) + 1);
            strcpy(tempPath, path);

            token2 = strtok(NULL, " ");
            char *port = token2;
            // printf("port = %s\n", port);

            // connect to the server
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(atoi(port));
            serv_addr.sin_addr.s_addr = inet_addr(host);

            if ((connect(sockfd, (struct sockaddr *)&serv_addr,
                         sizeof(serv_addr))) < 0)
            {
                perror("Unable to connect to server\n");
                exit(0);
            }

            // printf("tempPath = %s\n", tempPath);
            strcat(path, "/");
            strcat(path, filename);
            // printf("Path= %s\n", path);

            // content length is the legth of the file content after opening the file and reading it
            long long contentLength = 0;
            path[strlen(path) - 1] = '\0';
            FILE *fp = fopen(path, "r");
            if (fp == NULL)
            {
                printf("File not found\n");
                exit(0);
            }
            else
            {
                char c;
                // printf("file found\n");
                while ((c = fgetc(fp)) != EOF)
                {
                    contentLength++;
                }
            }
            // printf("ContentLen=%d\n", contentLength);
            fclose(fp);

            sprintf(cLength, "%d", contentLength); // convert string to int
            // printf("cLength= %d\n", cLength);

            char filePath[100];
            strcpy(filePath, path);
            // printf("filePath=%s\n", filePath);
            char *token3 = strtok(path, ".");
            token3 = strtok(NULL, " ");
            char *fileformat = token3;
            // printf("fileformat = %s\n", token3);
            // printf("tempPath = %s\n", tempPath);
            fileformat = chooseFileFormat(fileformat);
            // printf("Fileformatfjd= %s\n", fileformat);
            time_t t;
            time(&t);
            char *dateFormat = giveDate(dateFormat, t);
            // printf("dateFormat = %s\n", dateFormat);

            // printf("Path= %s\n", path);

            // now creating the HTTP request
            char request[BUFFERSIZE];

            strcpy(request, cmd);
            strcat(request, " /");
            strcat(request, filePath);
            strcat(request, " HTTP/1.1 \n");
            strcat(request, "Host: ");
            strcat(request, host);
            strcat(request, " \n");
            strcat(request, "Connection: close ");
            strcat(request, " \n");
            strcat(request, "Date: ");
            strcat(request, dateFormat);
            strcat(request, " \n");
            strcat(request, "Accept: ");
            strcat(request, fileformat);
            strcat(request, " \n");
            strcat(request, "Accept-Language: en-US,en;q=0.5 ");
            strcat(request, " \n");

            t -= 60 * 60 * 24 * 2;
            char *IfModified = giveDate(IfModified, t);

            strcat(request, "If-Modified-Since: ");
            strcat(request, dateFormat);

            strcat(request, " \n");
            strcat(request, "Content-language: en-US ");
            strcat(request, " \n");

            strcat(request, "Content-Length: ");
            strcat(request, cLength);
            strcat(request, " \n");

            strcat(request, "Content-Type: ");
            strcat(request, fileformat);
            strcat(request, " \n");

            // printf("\n%s\n", request);

            send(sockfd, request, strlen(request) + 1, 0);

            // waiting for the response from server
            struct pollfd fds[1];
            fds[0].fd = sockfd;
            fds[0].events = POLLIN;
            int ret = poll(fds, 1, 3000);
            if (ret == 0)
            {
                printf("Close the connection\n");
                close(sockfd);
                exit(0);
            }

            // PUT: reading the file and sending the data to server so that server will make a file at its side
            char response[BUFFERSIZE];
            recv(sockfd, response, BUFFERSIZE, 0);
            printf("**********Response from Server**********\n");
            printf("%s \n", response);

            // now sending the file content to server
            // char fileContent[100];
            fp = fopen(filePath, "r");

            // fseek(fp, 0, SEEK_SET);
            if (fp != NULL)
            {
                printf("File opened\n");
                char fileContent[100];
                int fileContent_size;
                while ((fileContent_size = fread(fileContent, 1, 100, fp)) > 0)
                {
                    int n = send(sockfd, fileContent, fileContent_size, 0);
                    // printf("n=%d\n",n);
                    // printf("fileContent = %s \n", fileContent);
                    for (int i = 0; i < 100; i++)
                        fileContent[i] = '\0';
                }
            }
            else
            {
                printf("File not found\n");
                exit(0);
            }
        }

        else if (strcmp(cmd, "GET") == 0)
        {
            // Returns first token
            char *token2 = strtok(url, "://");
            char *protocol = token2;
            // printf("protocol = %s\n", protocol);

            token2 = strtok(NULL, "/");
            char *host = token2;
            // printf("host = %s\n", host);

            token2 = strtok(NULL, ":");
            char *path = token2;
            char *tempPath = (char *)malloc(strlen(path) + 1);
            strcpy(tempPath, path);
            // printf("tempPath = %s\n", tempPath);

            token2 = strtok(NULL, ":");
            char *port = token2;
            // printf("port = %s\n", port);

            // connect to the server
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(atoi(port));
            serv_addr.sin_addr.s_addr = inet_addr(host);

            if ((connect(sockfd, (struct sockaddr *)&serv_addr,
                         sizeof(serv_addr))) < 0)
            {
                perror("Unable to connect to server\n");
                exit(0);
            }

            char *token3 = strtok(path, ".");
            // printf("fileformat = %s\n", token3);
            token3 = strtok(NULL, ":");
            char *fileformat = token3;
            // printf("fileformat = %s\n", token3);
            // printf("tempPath = %s\n", tempPath);
            fileformat = chooseFileFormat(fileformat);

            // printf("dateFormat = %s\n", dateFormat);
            time_t t;
            time(&t);
            char *dateFormat = giveDate(dateFormat, t);
            // printf("dateFormat = %s\n", dateFormat);

            // now creating the HTTP request
            char request[BUFFERSIZE];
            strcpy(request, cmd);
            strcat(request, " /");
            strcat(request, tempPath);
            strcat(request, " HTTP/1.1 \n");
            strcat(request, "Host: ");
            strcat(request, host);
            strcat(request, " \n");
            strcat(request, "Connection: close ");
            strcat(request, " \n");
            strcat(request, "Date: ");
            strcat(request, dateFormat);
            strcat(request, " \n");
            strcat(request, "Accept: ");
            strcat(request, fileformat);
            strcat(request, " \n");
            strcat(request, "Accept-Language: en-US,en;q=0.5 ");
            strcat(request, " \n");

            t -= 60 * 60 * 24 * 2;
            char *IfModified = giveDate(IfModified, t);

            strcat(request, "If-Modified-Since: ");
            strcat(request, dateFormat);

            strcat(request, " \n");
            strcat(request, "Content-language: en-US ");
            strcat(request, " \n");

            strcat(request, "Content-Type: ");
            strcat(request, fileformat);
            strcat(request, " \n");

            // printf("\n%s\n", request);

            // Accept-Language: en-US,en;q=0.5

            send(sockfd, request, strlen(request) + 1, 0); // request is sent *************************


            // waiting for the response from server
            struct pollfd fds[1];
            fds[0].fd = sockfd;
            fds[0].events = POLLIN;
            int ret = poll(fds, 1, 3000);
            if (ret == 0)
            {
                printf("Close the connection\n");
                close(sockfd);
                exit(0);
            }

            // receive the response from the server
            int i;
            char response[1024];
            for (i = 0; i < 1024; i++)
                response[i] = '\0';
            recv(sockfd, response, 1024, 0);
            printf("**********Response**********\n");
            printf("%s\n", response);

            // now we have to seperate the headers from the response
            char *token4 = strtok(response, " ");
            token4 = strtok(NULL, "\n");
            char *status = token4;
            // printf("status = %s\n", status);

            token4 = strtok(NULL, "\n");
            char *expr = token4;
            // printf("expr = %s\n", expr);

            token4 = strtok(NULL, "\n");
            char *cache = token4;
            // printf("cache = %s\n", cache);

            token4 = strtok(NULL, "\n");
            char *contLang = token4;
            // printf("contLang = %s\n", contLang);

            token4 = strtok(NULL, "\n");
            char *contType = token4;
            // printf("contType = %s\n", contType);

            // token4 = strtok(NULL, ":");
            // token4 = strtok(NULL, "\n");
            // char* type= token4;
            // printf("type = %s\n", type);

            token4 = strtok(NULL, "\n");
            char *lastModified = token4;
            // printf("lastModified = %s\n", lastModified);

            // now we have to write the entity body to a file
            // printf("path = %s\n", path);
            // printf("tempPath = %s\n", tempPath);
            char *token5 = strtok(tempPath, "/");
            char *fileFormat;
            while (token5 != NULL)
            {
                // printf("%s\n", token5);
                fileFormat = token5;
                token5 = strtok(NULL, "/");
            }

            // token5 = strtok(NULL, " ");

            // printf("fileFormat = %s\n", fileFormat);
            // char *MyfileName;
            // MyfileName = (char *)malloc(strlen(path) + 1);
            // strcpy(fileFormat, "DownloadedFile");
            // strcat(fileFormat, ".");
            // strcat(fileFormat, fileFormat);

            char entity_body[100];
            for (i = 0; i < 100; i++)
                entity_body[i] = '\0';

            FILE *fp = fopen(fileFormat, "w");
            if (fp != NULL)
            {
                int content_size;
                printf("Downloading file...\n");
                while ((content_size = recv(sockfd, entity_body, 100, 0)) > 0)
                {
                    int n = fwrite(entity_body, 1, content_size, fp);
                    // printf("n = %d\n", n);
                    // printf("entity_body = %s\n", entity_body);
                    for (i = 0; i < 100; i++)
                        entity_body[i] = '\0';
                }
                // int n= recv(sockfd, entity_body, 100, 0);

                
                printf("fileformat = %s\n", fileFormat);
                contType[strlen(contType) - 1] = '\0';
                if (fork() == 0)
                {
                    if (strcmp(contType, "Content-Type: image/jpeg") == 0)
                    {
                        printf("Opening image\n");
                        execlp("gthumb", "gthumb", fileFormat, NULL);
                    }
                    else
                    {
                        char *args[] = {"xdg-open", fileFormat, NULL};
                        execvp("xdg-open", args);
                    }
                    // printf("File downloaded successfully\n");
                }
                
            }
            fclose(fp);
        }
        else
        {
            printf("Invalid  command\n");
            continue;
        }

        close(sockfd);
    }
    return 0;
}

// GET http://127.0.0.1/Assignment3/lb.c:8080