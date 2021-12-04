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
    int fd = open(argv[2], O_RDONLY | O_SYNC);

    off_t num = FILESIZE / BLOCKSIZE;
    uint64_t start, end, total = 0;
    unsigned start_low, start_high, end_low, end_high;

    int i = 0;
    for (i = 0; i < num; i++) {
        off_t k = rand() % num;

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
        
        start = (((uint64_t)start_high << 32) | start_low);
        end = (((uint64_t)end_high << 32) | end_low);

        total += end - start;
    }
    close(fd);
    free(buf);

    double result = total / num ;
    printf("The cycles to read a 4KB data is %f \n",result);

    return 0;
}