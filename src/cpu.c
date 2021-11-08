#include <stdint.h>
#include "cpu.h"
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>

double get_read_overhead(int n)
{
    uint64_t start, end, total = 0;
    unsigned start_low, start_high, end_low, end_high;

    for (int i = 0; i < n; i++)
    {
        // preempt_disable();
        // raw_local_irq_save(flags);
        _mm_sfence();
        asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");

        
        asm volatile("RDTSCP\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     "CPUID\n\t"
                     : "=r"(end_high), "=r"(end_low)::"%rax", "%rbx", "%rcx", "%rdx");
        _mm_sfence();

        // raw_local_irq_restore(flags);
        // preempt_enable();

        start = (((uint64_t)start_high << 32) | start_low);
        end = (((uint64_t)end_high << 32) | end_low);

        // if (end < start) {
        //     printk("\n\tFatal error in src/cpu.c: start is larger than end. start:%llu, end:%llu", start, end);
        // }

        total += end - start;
    }

    return 1.0 * total / n;
}

double get_loop_overhead(int loop_count, int n)
{
    uint64_t start, end, total = 0;
    unsigned start_low, start_high, end_low, end_high;
    int i, j;

    for (i = 0; i < n; i++)
    {
        _mm_sfence();
        asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");

        for (j = 0; j < loop_count; j++)
            ;

        
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

    return ((1.0 * total / n) - get_read_overhead(n)) / loop_count;
}

static inline void test_no_param() {}
static inline void test_one_param(int a) {}
static inline void test_two_param(int a, int b) {}
static inline void test_three_param(int a, int b, int c) {}
static inline void test_four_param(int a, int b, int c, int d) {}
static inline void test_five_param(int a, int b, int c, int d, int e) {}
static inline void test_six_param(int a, int b, int c, int d, int e, int f) {}
static inline void test_seven_param(int a, int b, int c, int d, int e, int f, int g) {}

void get_procedure_overhead(double *results, int n)
{
    uint64_t start, end, total = 0;
    unsigned start_low, start_high, end_low, end_high;
    int i, a, b, c, d, e, f, g;

    double basic = get_read_overhead(n);

    // No parameters
    for (i = 0; i < n; i++)
    {
        _mm_sfence();
        asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");

        test_no_param();

        
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
    results[0] = 1.0 * total / n - basic;

    // One parameters
    for (i = 0; i < n; i++)
    {
        _mm_sfence();
        asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");

        test_one_param(0);

        
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
    results[1] = 1.0 * total / n - basic;

    // Two parameters
    for (i = 0; i < n; i++)
    {
        _mm_sfence();
        asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");

        test_two_param(0, 0);

        
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
    results[2] = 1.0 * total / n - basic;

    // Three parameters
    for (i = 0; i < n; i++)
    {
        _mm_sfence();
        asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");

        test_three_param(0, 0, 0);

        
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
    results[3] = 1.0 * total / n - basic;

    // Four parameters
    for (i = 0; i < n; i++)
    {
        _mm_sfence();
        asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");

        test_four_param(0, 0, 0, 0);

        
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
    results[4] = 1.0 * total / n - basic;

    // Five parameters
    for (i = 0; i < n; i++)
    {
        _mm_sfence();
        asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");

        test_five_param(0, 0, 0, 0, 0);

        
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
    results[5] = 1.0 * total / n - basic;

    // Six parameters
    for (i = 0; i < n; i++)
    {
        _mm_sfence();
        asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");

        test_six_param(0, 0, 0, 0, 0, 0);

        
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
    results[6] = 1.0 * total / n - basic;

    // Seven parameters
    for (i = 0; i < n; i++)
    {
        _mm_sfence();
        asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");

        test_seven_param(0, 0, 0, 0, 0, 0, 0);

        
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
    results[7] = 1.0 * total / n - basic;
}

double get_cycle_time(int n, int intSleepSec)
{
    uint64_t start, end, total = 0;
    unsigned start_low, start_high, end_low, end_high;

    for (int i = 0; i < n; i++)
    {
        _mm_sfence();
        asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");

        sleep(intSleepSec);

        
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

    return 1.0 * total / n / intSleepSec;
}

double get_syscall_overhead()
{
    int n = 1;

    uint64_t start, end, total = 0;
    unsigned start_low, start_high, end_low, end_high;

    for (int i = 0; i < n; i++)
    {
        _mm_sfence();
        asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");

        time(NULL);

        
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

    return 1.0 * total / n;
}

void *thread_start(void *arg)
{
    pthread_exit(0);
}

double get_thread_overhead(int n)
{
    pthread_t thread_id;
    int i = 0;
    uint64_t start, end, total = 0;
    unsigned start_low, start_high, end_low, end_high;
    for (i = 0; i < n; i++)
    {
        _mm_sfence();
        asm volatile("RDTSCP\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     "CPUID\n\t"
                     : "=r"(end_high), "=r"(end_low)::"%rax", "%rbx", "%rcx", "%rdx");

        pthread_create(&thread_id, NULL, thread_start, NULL);

        
        asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");
        _mm_sfence();
        start = (((uint64_t)start_high << 32) | start_low);
        end = (((uint64_t)end_high << 32) | end_low);

        total += end - start;
    }
    return 1.0 * total / n;
}



double get_process_overhead(int n)
{
    int i = 0;
    uint64_t start, end, total = 0;
    unsigned start_low, start_high, end_low, end_high;
    for (i = 0; i < n; i++)
    {
        pid_t pid;
        _mm_sfence();
        asm volatile("RDTSCP\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     "CPUID\n\t"
                     : "=r"(end_high), "=r"(end_low)::"%rax", "%rbx", "%rcx", "%rdx");

        if ((pid = fork()) == -1)
        {
            handle_error("fork");
        }
        else if (pid == 0)
        {
            // child, exit
            exit(-1);
        }
        else
        {
            wait(NULL);
        }
        
        asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");
        _mm_sfence();
        start = (((uint64_t)start_high << 32) | start_low);
        end = (((uint64_t)end_high << 32) | end_low);

        total += end - start;
    }
    return 1.0 * total / n;
}
