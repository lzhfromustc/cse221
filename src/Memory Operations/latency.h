#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <xmmintrin.h>

// #define DEBUG 1

typedef struct {
    uint64_t result; // the measured latency
    uint64_t size; // the size that is actually copied
} Result;


Result lat_mem(uint64_t);
void garbage_use_ptr(void *);
static volatile uint64_t garbage_usage;
int measure_mem_lat(uint64_t, uint64_t, uint64_t);