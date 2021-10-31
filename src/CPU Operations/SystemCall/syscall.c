#include "../../cpu.h"
#include <stdio.h>
#include <time.h>
#include <stdint.h>

double get_syscall_overhead() {
    printf("Getting syscall overhead \n");
    int n = 1;

    uint64_t start, end, total = 0;
    unsigned start_low, start_high, end_low, end_high;


    for(int i = 0; i < n; i++){
        
        asm volatile (  "CPUID\n\t"
                        "RDTSC\n\t"
                        "mov %%edx, %0\n\t"
                        "mov %%eax, %1\n\t": "=r" (start_high), "=r" (start_low)::
"%rax", "%rbx", "%rcx", "%rdx");

        time(NULL);


        asm volatile(   "RDTSCP\n\t"
                        "mov %%edx, %0\n\t"
                        "mov %%eax, %1\n\t"
                        "CPUID\n\t": "=r" (end_high), "=r" (end_low)::
"%rax", "%rbx", "%rcx", "%rdx");
        

        start = ( ((uint64_t)start_high << 32) | start_low );
        end = ( ((uint64_t)end_high << 32) | end_low );


        total += end - start; 
    }

    return 1.0 * total / n;
}