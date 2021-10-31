#include <pthread.h>

void * thread_start(void *arg) {
    pthread_exit(0);
}
double get_thread_overhead(int n){
    pthread_t thread_id;
    int i = 0;
    uint64_t start, end, total = 0;
    unsigned start_low, start_high, end_low, end_high;
    for(i = 0;i < n; i++)    
    asm volatile(   "RDTSCP\n\t"
                        "mov %%edx, %0\n\t"
                        "mov %%eax, %1\n\t"
                        "CPUID\n\t": "=r" (end_high), "=r" (end_low)::
"%rax", "%rbx", "%rcx", "%rdx");

    pthread_create(&thread_id, NULL, thread_start, NULL);

        asm volatile (  "CPUID\n\t"
                        "RDTSC\n\t"
                        "mov %%edx, %0\n\t"
                        "mov %%eax, %1\n\t": "=r" (start_high), "=r" (start_low)::
"%rax", "%rbx", "%rcx", "%rdx");
        start = ( ((uint64_t)start_high << 32) | start_low );
        end = ( ((uint64_t)end_high << 32) | end_low );

        total += end - start;
    } 
    return 1.0 * total / n;
}

double get_process_overhead(int n){
    int i = 0;
    uint64_t start, end, total = 0;
    unsigned start_low, start_high, end_low, end_high;
    for(i = 0;i < n; i++)
    pid_t pid;
    asm volatile(   "RDTSCP\n\t"
                        "mov %%edx, %0\n\t"
                        "mov %%eax, %1\n\t"
                        "CPUID\n\t": "=r" (end_high), "=r" (end_low)::
"%rax", "%rbx", "%rcx", "%rdx");

    if ((pid = fork()) == -1) {
            handle_error("fork");
        } else if (pid == 0) {
            // child, exit
            exit(-1);
        } else {
            wait(NULL);
        }

        asm volatile (  "CPUID\n\t"
                        "RDTSC\n\t"
                        "mov %%edx, %0\n\t"
                        "mov %%eax, %1\n\t": "=r" (start_high), "=r" (start_low)::
"%rax", "%rbx", "%rcx", "%rdx");
        start = ( ((uint64_t)start_high << 32) | start_low );
        end = ( ((uint64_t)end_high << 32) | end_low );

        total += end - start;
    } 
    return 1.0 * total / n;
}

