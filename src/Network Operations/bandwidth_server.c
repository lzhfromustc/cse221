#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void bw_server(char *addr, unsigned short port, int bytes, int n)
{
    printf("%d\n", bytes);
    struct sockaddr_in serv_addr;
    int serv_fd, accept_fd, serv_opt = 1, serv_addr_len = sizeof(serv_addr), i;
    char* buffer = (char*)malloc(bytes);
    char* message = (char*)malloc(bytes);

    if ((serv_fd = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    {
        perror("ERROR opening socket");
        exit(EXIT_FAILURE);
    }

    // if (setsockopt(serv_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &serv_opt, sizeof(serv_opt)) < 0)
    // {
    //     perror("ERROR setting sockopt");
    //     exit(EXIT_FAILURE);
    // }


    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(addr);
    serv_addr.sin_port = htons(port);

    if (bind(serv_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR binding addr");
        exit(EXIT_FAILURE);
    }

    if (listen(serv_fd, 3) < 0){
        perror("ERROR listening addr");
        exit(EXIT_FAILURE);
    }
    
    while(1)
    {
        if ((accept_fd = accept(serv_fd, (struct sockaddr *)&serv_addr, (socklen_t*)&serv_addr_len))< 0)
        {
            perror("ERROR accepting connection");
            exit(EXIT_FAILURE);
        }
        for(i = 0; i < n; i++)
        {
            // read(accept_fd, buffer, bytes);
            send(accept_fd, message, bytes, 0);
        }
        close(accept_fd);
    }
    close(serv_fd);
    free(message);
    free(buffer);
}

int main(){
    char* addr = "0.0.0.0";
    unsigned short port = 12580;

    bw_server(addr, port, 1 << 10, 10000);

    return 0;
}