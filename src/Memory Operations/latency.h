#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <xmmintrin.h>

// #define DEBUG 1

typedef struct {
    uint64_t result; // the measured latency
    uint64_t size; // the size that is actually copied
} Result;

// The garbage and vec_int are too large, so we moved them outside
#define SIZE_GARBAGE 2<<20 // 16MB is definitely larger than our cache
char garbage1[SIZE_GARBAGE];
char garbage2[SIZE_GARBAGE];

Result lat_mem(uint64_t);
void garbage_use_ptr(void *);
static volatile uint64_t garbage_usage;
int measure_mem_lat(uint64_t, uint64_t, uint64_t);