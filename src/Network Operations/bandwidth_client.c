#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

double bw_client(char* addr, unsigned short port, int bytes, unsigned long freq, int n)
{
    uint64_t start, end, min = INT_MAX;
    unsigned start_low, start_high, end_low, end_high;
    int sockfd, i;
    struct sockaddr_in serveraddr;
    char* buffer = (char*)malloc(bytes);
    char* message = (char*)malloc(bytes);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(addr);
    serveraddr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)))
    {
        printf("ERROR: connection failed!\n");
        return -1;
    }

    for(i = 0; i < n; i++){
        asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");

        // send(sockfd, message, bytes, 0);
        read(sockfd, buffer, bytes);

        asm volatile("RDTSCP\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     "CPUID\n\t"
                     : "=r"(end_high), "=r"(end_low)::"%rax", "%rbx", "%rcx", "%rdx");
        
        start = (((uint64_t)start_high << 32) | start_low);
        end = (((uint64_t)end_high << 32) | end_low);
        min = end - start < min ? end - start : min;
    }
    close(sockfd);
    free(message);
    free(buffer);
    
    printf("%d, %lu\n", bytes, freq);

    return (bytes * 1.0 / 1000000) * freq / min;
}

int main(){
    char* addr = "127.0.0.1";
    unsigned short port = 12580;
    
    double cycles = bw_client(addr, port, 1 << 20, 3500000000, 100);
    printf("%.2f\n", cycles);

    return 0;
}
