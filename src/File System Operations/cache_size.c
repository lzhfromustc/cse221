#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>
#include <xmmintrin.h>

const off_t BLOCKSIZE = 4*1024;

int main(int argc, const char * argv[])
{
    void* buf = malloc(BLOCKSIZE);
	const off_t FILESIZE = atoll(argv[1]);
    int fd = open(argv[2], O_RDONLY | O_SYNC);

    //We first seek to the last block in the file
    lseek(fd, FILESIZE - 1, SEEK_SET);
    off_t totalBytes = 0;

    //Then we read the file from end to start to put the file into cache
    while(1){
        lseek(fd, -2*BLOCKSIZE, SEEK_CUR);
        ssize_t bytes = read(fd, buf, BLOCKSIZE);
        if (bytes <= 0 || totalBytes >= FILESIZE)
            break;
        totalBytes += bytes;
    }
    
    close(fd);

    //We close the file and read it again
    fd = open(argv[2], O_RDONLY| O_SYNC);
    totalBytes = 0;

    //Still seek the last block in the file
    lseek(fd, FILESIZE - 1, SEEK_SET);
    
    uint64_t start, end, total = 0;
    unsigned start_low, start_high, end_low, end_high;

    // Read backwards to avoid prefetching the blocks
    while(1){
        lseek(fd, -2*BLOCKSIZE, SEEK_CUR);
        
        asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");

        ssize_t bytes = read(fd, buf, BLOCKSIZE);

        asm volatile("RDTSCP\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     "CPUID\n\t"
                     : "=r"(end_high), "=r"(end_low)::"%rax", "%rbx", "%rcx", "%rdx");
        //_mm_sfence();

        start = (((uint64_t)start_high << 32) | start_low);
        end = (((uint64_t)end_high << 32) | end_low);

        total += end - start;
        if (bytes <= 0 || totalBytes >= FILESIZE)
            break;
        totalBytes += bytes;
    }
    
    close(fd);
    free(buf);
    double ans = (double)(total / (FILESIZE / BLOCKSIZE)) - 32;
    printf("%lf\n",ans);
    return 0;
}