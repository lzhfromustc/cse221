#include <stdio.h>
// #include "latency.h"
// #include "bandwidth.h"
#include "pagefault.h"

int main() {

    // 1. Memory latency

    printf("=====Starting memory latency measurement\n");

    // parameters
    uint64_t num_dot = 40; // number of maximum data points in the figure; default: 40 points
    uint64_t min_size = 1<<8; // the minimal linked list size; default: 256 B; Note: we will ignore the first 4 data points
    uint64_t max_size = 128 * 1<<20; // the maximum linked list size; default: 128 MB
    
    measure_mem_lat(num_dot, min_size, max_size);

    printf("=====Finished memory latency measurement\n\n\n");




    // 2. Memory bandwidth

    printf("=====Starting memory bandwidth measurement\n");

    // parameters
    uint64_t iteration = 10; // Number of iterations of the whole measurement; default: 10
    
    measure_bw(iteration);

    printf("=====Finished memory bandwidth measurement\n\n\n");




    // 3. Page fault

    printf("=====Starting page fault measurement\n");

    // parameters
    
    uint64_t size_file = 64 << 20; // 64MB file
    uint64_t iteration_fault = 10;
    uint64_t num_step = 100;
    uint64_t size_step = 10 * 8192; // our page size should be 512 to 8192 bytes
    char filename[] = "testFile.txt";

    
    measure_page_fault(size_file, iteration_fault, num_step, size_step, filename);

    printf("=====Finished page fault measurement\n\n\n");


}


