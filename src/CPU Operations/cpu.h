#include <stdlib.h>
#ifndef CPU_H
#define CPU_H

#define handle_error(msg) \
       do { perror(msg); exit(EXIT_FAILURE); } while (0)

double get_read_overhead(int n);
double get_loop_overhead(int loop_count, int n);
void get_procedure_overhead(double* results, int n);
double get_thread_overhead(int n);
double get_process_oeverhead(int n);
double get_cycle_time(int n, int intSleepSec);
double get_syscall_overhead();

#endif
