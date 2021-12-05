#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

double setup_client(char* addr, unsigned short port, int n)
{
    uint64_t start, end, total = 0;
    unsigned start_low, start_high, end_low, end_high;
    int sockfd, i;
    struct sockaddr_in serveraddr;
    char buffer[64] = {0};
    char message[64] = "Hello from client!";

    for(i = 0; i < n; i++){
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("\n Socket creation error \n");
            return -1;
        }

        serveraddr.sin_family = AF_INET;
        serveraddr.sin_addr.s_addr = inet_addr(addr);
        serveraddr.sin_port = htons(port);

        asm volatile("CPUID\n\t"
                    "RDTSC\n\t"
                    "mov %%edx, %0\n\t"
                    "mov %%eax, %1\n\t"
                    : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");

        if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)))
        {
            printf("ERROR: connection failed!\n");
            return -1;
        }

        asm volatile("RDTSCP\n\t"
                        "mov %%edx, %0\n\t"
                        "mov %%eax, %1\n\t"
                        "CPUID\n\t"
                        : "=r"(end_high), "=r"(end_low)::"%rax", "%rbx", "%rcx", "%rdx");
        
        send(sockfd, message, sizeof(char), 0);
        read(sockfd, buffer, sizeof(char));
        
        start = (((uint64_t)start_high << 32) | start_low);
        end = (((uint64_t)end_high << 32) | end_low);
        total += end - start;        
        close(sockfd);
        sleep(1);
    }

    return 1.0 * total / n;
}

double teardown_client(char* addr, unsigned short port, int n)
{
    uint64_t start, end, total = 0;
    unsigned start_low, start_high, end_low, end_high;
    int sockfd, i;
    struct sockaddr_in serveraddr;
    char buffer[64] = {0};
    char message[64] = "Hello from client!";

    for(i = 0; i < n; i++){
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

        send(sockfd, message, sizeof(char), 0);
        read(sockfd, buffer, sizeof(char));

        asm volatile("CPUID\n\t"
                    "RDTSC\n\t"
                    "mov %%edx, %0\n\t"
                    "mov %%eax, %1\n\t"
                    : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");
        
        close(sockfd);

        asm volatile("RDTSCP\n\t"
                        "mov %%edx, %0\n\t"
                        "mov %%eax, %1\n\t"
                        "CPUID\n\t"
                        : "=r"(end_high), "=r"(end_low)::"%rax", "%rbx", "%rcx", "%rdx");

        start = (((uint64_t)start_high << 32) | start_low);
        end = (((uint64_t)end_high << 32) | end_low);
        total += end - start;
        sleep(1);
    }


    return 1.0 * total / n;
}

int main(){
    char* addr = "100.81.36.170 ";
    unsigned short port = 9999;
    
    double cycles = setup_client(addr, port, 100);
    printf("%.2f\n", cycles);
    printf("%.4f ms\n", cycles * 100 / 3600000000);

    cycles = teardown_client(addr, port, 100);
    printf("%.2f\n", cycles);
    printf("%.4f ms\n", cycles * 100 / 3600000000);

    return 0;
}