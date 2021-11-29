#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>
#include <xmmintrin.h>
#include <errno.h>

extern int errno;

const off_t BLOCKSIZE = 4*1024;
off_t FILESIZE;

double avg_seq_time(char *file);

double avg_random_time(char *file);

int main(int argc, const char *argv[]) //input the filesize and name of file
{
    FILESIZE = atoll(argv[1]);
    srand((unsigned int)time(NULL));



    
    double seq_ans = avg_seq_time(argv[2]);
    
    double ran_ans = avg_random_time(argv[2]);
    printf("%.4lf \n", seq_ans); // output 21 bits after point
    printf("%.4lf \n", ran_ans); 
    return 0;
}

double avg_seq_time(char *file)
{
    int fd = open(file, O_SYNC | O_DIRECT);
    // if(fcntl(fd, F_NOCACHE, 1) == -1) {
    //     printf("Failed.\n"); //cannot disable cache
    // }

    printf("fd = %d \n",fd);

    void *buf = malloc(BLOCKSIZE);
    uint64_t start, end, total = 0;
    unsigned start_low, start_high, end_low, end_high;
    
    errno = 0;
    ssize_t bytes = read(fd, buf, BLOCKSIZE); // return #byte when read successfully

    if (bytes <= 0) {
            printf("bytes = %ld \n", bytes);
            printf("Cannot open the file in cp1 \n");
            printf("\n");
    }

    if (errno!= 0){
            printf("errno值: %d\n",errno);
            printf("错误信息：%s\n",strerror(errno));
    }


    while (1) {

        _mm_sfence();
        asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");

        ssize_t bytes = read(fd, buf, BLOCKSIZE); // return #byte when read successfully
        if (bytes <= 0) {
            printf("Cannot open the file in cp2 \n");
            printf("bytes = %ld \n", bytes);
            break;
        }

        asm volatile("RDTSCP\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     "CPUID\n\t"
                     : "=r"(end_high), "=r"(end_low)::"%rax", "%rbx", "%rcx", "%rdx");
        _mm_sfence();

        start = (((uint64_t)start_high << 32) | start_low);
        end = (((uint64_t)end_high << 32) | end_low);
        
        total += end - start;
    }
    close(fd);
    double num = FILESIZE / BLOCKSIZE;
    free(buf);
    return total / num / 3.6 / 1000;
}

double avg_random_time(char *file)
{
    int i = 0;
    int fd = open(file, O_SYNC | O_DIRECT); //open synchronously
    // if(fcntl(fd, F_NOCACHE, 1) == -1) //disable cache
    // {
    //     printf("Failed.\n"); //cannot disable cache
    // }
    off_t num = FILESIZE / BLOCKSIZE;
    void *buf = malloc(BLOCKSIZE);
    uint64_t start, end, total = 0;
    unsigned start_low, start_high, end_low, end_high;

    for (i = 0; i < num; i++) {
        off_t k = rand() % num;

        _mm_sfence();
        asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");


        lseek(fd, k * BLOCKSIZE, SEEK_SET); // offset
        ssize_t bytes = read(fd, buf, BLOCKSIZE);
        if (bytes <= 0) {
            printf("Cannot open the file in cp3 \n");
            printf("bytes = %ld \n", bytes);
            break;
        }
        asm volatile("RDTSCP\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     "CPUID\n\t"
                     : "=r"(end_high), "=r"(end_low)::"%rax", "%rbx", "%rcx", "%rdx");
        _mm_sfence();
        
        start = (((uint64_t)start_high << 32) | start_low);
        end = (((uint64_t)end_high << 32) | end_low);

        total += end - start;
    }
    close(fd);
    free(buf);
    return total / (double)num / 3.6 / 1000;
}