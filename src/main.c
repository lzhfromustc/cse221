#include <stdio.h>
#include "cpu.h"
int main(){
    // Get read overhead
    double overhead;
    double overheads[8];

    overhead = get_read_overhead(1000000);
    printf("%f\n", overhead);

    overhead = get_loop_overhead(1000, 1000000);
    printf("%f\n", overhead);

    get_procedure_overhead(overheads, 1000000);
    for(int i = 0; i < 8; i++){
        printf("%f ", overheads[i]);
    }
    putchar('\n');
    

    // double cycleTime;
    // cycleTime = get_cycle_time(5, 10);
    // printf("Number of cycles per second:%f\n", cycleTime);

    double sysOverhead;
    sysOverhead = get_syscall_overhead();
    printf("System call overhead of time(NULL):%f\n", sysOverhead);

    overhead = get_thread_overhead(10);    
    printf("%f\n", overhead);

    overhead = get_process_overhead(10) ;   
    printf("%f\n", overhead);

    return 0;
}
