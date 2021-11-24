#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <client.h>

double get_round_trip_time(char* addr, unsigned short port)
{
    int sockfd;
    struct sockaddr_in serveraddr;
    char buffer[64] = {0};
    char message[64] = "Hello from client!";

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, addr, &(serveraddr.sin_addr));
    serveraddr.sin_port = (unsigned short)(port);

    if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)))
    {
        printf("ERROR: connection failed!\n");
        return -1;
    }
    send(sockfd, message, strlen(message), 0);
    printf("Messge sent\n");
    read(sockfd, buffer, 64);
    printf("Receive: %s\n", buffer);
    close(sockfd);
    return 0;
}