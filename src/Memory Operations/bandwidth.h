#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <xmmintrin.h>

int measure_bw(uint64_t);
static volatile uint64_t garbage_int_usage;
void garbage_use_int(int);