#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>
#include <xmmintrin.h>

const off_t BLOCKSIZE = 4*1024;

int main(int argc, const char *argv[]){
	int n = atoll(argv[1]);
	pid_t pids[n];
	unsigned sum = 0;
	for(int i=0; i<n; i++)
	{
		pids[i] = fork();
		if(pids[i] < 0)
		{
			perror("fork");
			abort();
		}
		else if(pids[i] == 0)
		{
			readFile(i);
			exit(0);
		}
	}
}

void readFile(int i)
{
	void* buffer = malloc(BLOCKSIZE);
	char* files[] = {"file_4_read/size64k_0", "file_4_read/size64k_1", "file_4_read/size64k_2", "file_4_read/size64k_3", "file_4_read/size64k_4", "file_4_read/size64k_5", "file_4_read/size64k_6", "file_4_read/size64k_7","file_4_read/size64k_8","file_4_read/size64k_9","file_4_read/size64k_10"};
	int fd = open(files[i], O_RDONLY | O_SYNC);
	const off_t FILESIZE = 64 * 1024;
    uint64_t start, end, total = 0;
    unsigned start_low, start_high, end_low, end_high;
    
	while(1)
	{
		asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");
		ssize_t bytes = read(fd, buffer, BLOCKSIZE);
		if(bytes <= 0){
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
    free(buffer);
	double num = FILESIZE / BLOCKSIZE;
    double result = total / num ;

    printf("The cycles to read a 4KB data is %f \n",result);
}