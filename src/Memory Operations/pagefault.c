#include "pagefault.h"

int measure_page_fault(uint64_t size_file, uint64_t iteration, uint64_t num_step, uint64_t size_step, char *filename) {
   
    if (num_step * size_step >= size_file) {
        printf("Error: the size of file is too small, we will step out of file\n");
        printf("File size:%lu\tStep number: %lu\tStep size: %lu", size_file, num_step, size_step);
    }

    // Step 1. create file and set size
    FILE *fp=fopen(filename, "w");
    if (fp == NULL) {
        printf("Failed to open the file\n");
        exit(-1);
    }
    if (ftruncate(fileno(fp), size_file) == -1) {
        printf("Failed to change the file's size to %lu\n", size_file);
        exit(-1);
    }

    // Plan A: Simply get file descriptor from fp is failed. No idea why
    // int file_descriptor = fileno(fp);

    // Plan B: Reopen this file and get its file descriptor. This worked
    // This open, size, and mmap code is from https://www.lemoda.net/c/mmap-example/

    // Full path to the file
    char *full_file_path=(char *)malloc(1000*sizeof(char));
    getcwd(full_file_path,100);
    strcat(full_file_path, "/");
    strcat(full_file_path, filename);

    // File descriptor
    int file_descriptor = open(full_file_path, O_RDONLY);
    struct stat stat_file;
    if (file_descriptor < 0) {
        printf("Failed to reopen:%s\n", full_file_path);
        exit(-1);
    }

    // Now check whether the size of the file is still size_file
    int status = fstat (file_descriptor, &stat_file);
    if (status < 0) {
        printf("Failed to fstat file:%s\n", full_file_path);
        exit(-1);
    } 
    uint64_t cur_size_file = (uint64_t) stat_file.st_size;
    if (cur_size_file != size_file) {
        printf("After reopen, the size is not matching. Prev size: %lu\tCur size:%lu\n", size_file, cur_size_file);
        exit(-1);
    } 
    printf("Created a file of size %lu bytes\nFull path of the file:%s\n", size_file, full_file_path);


    uint64_t total = 0;

    // Iteration:
    for (int i = 0; i < iteration; i++) {

        // Step 2: Flush page cache
        // TODO

        // Step 3: Map file to memory
        // TODO: checkout the meaning of parameters
        void *mmap_result = mmap(0, size_file, PROT_READ, MAP_PRIVATE, file_descriptor, 0); // parameters from https://www.lemoda.net/c/mmap-example/
        if (mmap_result == MAP_FAILED) {
            printf("Failed to mmap the file\n");
            exit(-1);
        }

        char *read_mem_head = (char*) mmap_result;
        char c0 = read_mem_head[0];
        char c1 = read_mem_head[1];


        // Step 4: Disable page prefetch
        // TODO

        // Step 5: Fill cache with garbage
        memcpy(garbage1, garbage2, SIZE_GARBAGE);

        // Step 6: Measurement

        // variables for the cycle count
        uint64_t start, end = 0;
        unsigned start_low, start_high, end_low, end_high;

        // variables for the access loop
        register int j;

        // sfence and measurement
        _mm_sfence();
        asm volatile("CPUID\n\t"
                        "RDTSC\n\t"
                        "mov %%edx, %0\n\t"
                        "mov %%eax, %1\n\t"
                        : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");
        
        
        for (j = 0; j < num_step; j++) {
            // since page fault's time should be much higher than loop, we don't unroll the loop here
            garbage_char_usage = read_mem_head[j * size_step]; 
        }

        // sfence and measurement
        
        asm volatile("RDTSCP\n\t"
                        "mov %%edx, %0\n\t"
                        "mov %%eax, %1\n\t"
                        "CPUID\n\t"
                        : "=r"(end_high), "=r"(end_low)::"%rax", "%rbx", "%rcx", "%rdx");
        _mm_sfence();

        start = (((uint64_t)start_high << 32) | start_low);
        end = (((uint64_t)end_high << 32) | end_low);
        total += end - start;

        // Step 7: unmap file
        munmap(read_mem_head, size_file);
    }

    total = total / iteration / num_step;

    // Step 8: Print results
    printf("The average CPU cycles for each page fault:%lu\n", total);

    // Step 9: Close file
    fclose(fp);
}