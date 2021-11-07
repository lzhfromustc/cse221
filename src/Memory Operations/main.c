#include <stdio.h>
// #include "latency.h"
#include "bandwidth.h"

int main() {

    // 1. Memory latency

    // printf("Starting memory latency measurement\n");

    // // parameters
    // uint64_t num_dot = 15; // number of maximum data points in the figure; default: 15 points
    // uint64_t min_size = 1<<8; // the minimal linked list size; default: 256 B
    // uint64_t max_size = 16<<20; // the maximum linked list size; default: 16 MB
    
    // measure_mem_lat(num_dot, min_size, max_size);

    // printf("Finished memory latency measurement\n");




    // 2. Memory bandwidth

    printf("Starting memory bandwidth measurement\n");

    // parameters
    uint64_t iteration = 10; // Number of iterations of the whole measurement; default: 10
    
    measure_bw(iteration);

    printf("Finished memory bandwidth measurement\n");

}

