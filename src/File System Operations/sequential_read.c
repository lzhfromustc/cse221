#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>
#include <xmmintrin.h>

const off_t BLOCKSIZE = 4*1024;

int main(int argc, const char *argv[]) //input the filesize and name of file
{
    void* buf = malloc(BLOCKSIZE);
	const off_t FILESIZE = atoll(argv[1]);

    uint64_t start, end, total = 0;
    unsigned start_low, start_high, end_low, end_high;
    

    int fd = open(argv[2], O_RDONLY| O_SYNC);


    //lseek(fd, FILESIZE - 1, SEEK_SET);
    while (1) {
        
        asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");

        ssize_t bytes = read(fd, buf, BLOCKSIZE);
        if (bytes <= 0){
            printf("Finish read the file \n");
            break;
        }

        asm volatile("RDTSCP\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     "CPUID\n\t"
                     : "=r"(end_high), "=r"(end_low)::"%rax", "%rbx", "%rcx", "%rdx");

        start = (((uint64_t)start_high << 32) | start_low);
        end = (((uint64_t)end_high << 32) | end_low);
        
        total += end - start;
    }
    close(fd);
    double num = FILESIZE / BLOCKSIZE;
    free(buf);

    double result = total / num ;
    printf("The cycles to read a 4KB data is %f \n",result);

    return 0;
}