#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>


double round_trip_server(char* addr, int port){
    /*
    Domain： AF_INET IPV4
    Type: SOCK_STREAM TCP
    Protocol: 0 IP
    */ 
    int sockfd, ret;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0)
    if (sockfd < 0){
        perror("ERROR opening socket");
        exit(-1);
    }
    
    sock_addr = new struct sockaddr()
    ret = bind(sockfd, );
    if(ret < 0){
        
    }
}