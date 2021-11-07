#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <xmmintrin.h>

#define DEBUG 1

uint64_t lat_mem(uint64_t);
void garbage_use_ptr(void *);
static volatile uint64_t garbage_usage;
int measure_mem_lat(uint64_t, uint64_t, uint64_t);