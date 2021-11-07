#include "latency.h"

int measure_bw(uint64_t);
static volatile uint64_t garbage_int_usage;
void garbage_use_int(int);

// The garbage and vec_int are too large, so we moved them outside
#define SIZE_GARBAGE 2<<20 // 16MB is definitely larger than our cache
char garbage1[SIZE_GARBAGE];
char garbage2[SIZE_GARBAGE];
#define LEN_VEC_INT 1<<20 // length of int array
#define CPU_FREQUENCY 3.6 * 1024 * 1024 * 1024 // our CPU frequency is 3.6 GHz