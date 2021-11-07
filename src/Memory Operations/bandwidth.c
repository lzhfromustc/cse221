#include "bandwidth.h"

// The garbage and vec_int are too large, so we moved them outside
#define SIZE_GARBAGE 2<<20 // 16MB is definitely larger than our cache
char garbage1[SIZE_GARBAGE];
char garbage2[SIZE_GARBAGE];
#define LEN_VEC_INT 1<<20 // length of int array
#define CPU_FREQUENCY 3.6 * 1024 * 1024 * 1024 // our CPU frequency is 3.6 GHz

int measure_bw(uint64_t iteration) {
    // Step 1: Create the array of int, each int is 4 bytes
    
    static int vec_int[LEN_VEC_INT];
    for (int i = 0; i < LEN_VEC_INT; i++) {
        vec_int[i] = 3 * i; // can replace with any calculation
    }
    
    // Step 2: Fill cache with garbage
    memcpy(garbage1, garbage2, SIZE_GARBAGE);

    // Step 3: measure the unrolled loop

    uint64_t total = 0;

    for (int i = 0; i < iteration; i++) {
        // variables for the cycle count
        uint64_t start, end = 0;
        unsigned start_low, start_high, end_low, end_high;

        // variables for the sum loop
        int sum = 0;
        register int count_128 = LEN_VEC_INT / 128;
        register int count_rest = LEN_VEC_INT % 128;
        register int j;
        register int *last_ptr = &vec_int[LEN_VEC_INT-1];

        // sfence and measurement
        _mm_sfence();
        asm volatile("CPUID\n\t"
                        "RDTSC\n\t"
                        "mov %%edx, %0\n\t"
                        "mov %%eax, %1\n\t"
                        : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");

        for (j = 0; j < count_128; j++) {
            // The following code comes from http://www.bitmover.com/lmbench/bw_mem_rd.8.html
            register int *p = vec_int;
            while (p < last_ptr) {
                #define	DOIT(z)	sum += p[z];
                DOIT(0) DOIT(4) DOIT(8) DOIT(12) DOIT(16) DOIT(20) DOIT(24)
                DOIT(28) DOIT(32) DOIT(36) DOIT(40) DOIT(44) DOIT(48) DOIT(52)
                DOIT(56) DOIT(60) DOIT(64) DOIT(68) DOIT(72) DOIT(76)
                DOIT(80) DOIT(84) DOIT(88) DOIT(92) DOIT(96) DOIT(100)
                DOIT(104) DOIT(108) DOIT(112) DOIT(116) DOIT(120) DOIT(124);
                p +=  128;
            }
        }

        #undef	DOIT

        // sfence and measurement
        _mm_sfence();
        asm volatile("RDTSCP\n\t"
                        "mov %%edx, %0\n\t"
                        "mov %%eax, %1\n\t"
                        "CPUID\n\t"
                        : "=r"(end_high), "=r"(end_low)::"%rax", "%rbx", "%rcx", "%rdx");

        garbage_use_int(sum); // prevent compiler from optimize out our code

        start = (((uint64_t)start_high << 32) | start_low);
        end = (((uint64_t)end_high << 32) | end_low);
        total += end - start;
    }

    float cycles_per_LEN_VEC_INT = total / iteration ;

    // Step 4: print results
    float size_in_MB = (4 * LEN_VEC_INT)>>20;
    float bw_MB_per_S = size_in_MB / (cycles_per_LEN_VEC_INT / (float)(CPU_FREQUENCY));
    printf("Our int size is:%lu\n", sizeof(int));
    printf("The bandwidth (CPU cycle) of reading %f MB in main memory:%f\n", size_in_MB, cycles_per_LEN_VEC_INT);
    printf("Which is %f MB/s\n", bw_MB_per_S);

    printf("Please ignore this number:%lu\n",garbage_int_usage);

    return 0;
}

void garbage_use_int(int sum) {
    garbage_int_usage += sum;
}