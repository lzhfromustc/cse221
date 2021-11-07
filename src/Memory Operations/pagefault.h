#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include "bandwidth.h"

int measure_page_fault(uint64_t, uint64_t, uint64_t, uint64_t, char *);
static volatile char garbage_char_usage;