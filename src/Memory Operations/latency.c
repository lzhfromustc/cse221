#include "latency.h"



int measure_mem_lat(uint64_t num_dot, uint64_t min_size, uint64_t max_size) {

    // set sizes of multiple LLists
    // grow in 2^n
    uint64_t vec_size_LList[num_dot];
    for (int i = 0; i < num_dot; i++) {
        uint64_t size = min_size * 1<<i;
        if (size > max_size) {
            vec_size_LList[i] = max_size;
            break;
        }
        vec_size_LList[i] = size;
    }

    ///DEBUG
    #ifdef DEBUG
    printf("vec_size_LList:");
    for (int i = 0; i < num_dot; i++) {
        printf("\t%lu", vec_size_LList[i]);
    }
    printf("\n");
    #endif
    


    // obtain the latency of each size
    uint64_t vec_latency[num_dot];
    uint64_t vec_ave_latency[num_dot];
    uint64_t prev_latency = 0;
    uint64_t prev_size = 0;
    for (int i = 0; i < num_dot; i++) {
        vec_latency[i] = lat_mem(vec_size_LList[i]);
        vec_ave_latency[i] = 1000 * (vec_latency[i] - prev_latency) / (vec_size_LList[i] - prev_size);
        prev_latency = vec_latency[i];
        prev_size = vec_size_LList[i];
    }

    // print result
    printf("The size_of_linked_list:");
    for (int i = 0; i < num_dot; i++) {
        printf(" %lu", vec_size_LList[i]);
    }
    printf("\n");
    printf("The total latency_of_mem_read:");
    for (int i = 0; i < num_dot; i++) {
        printf(" %lu", vec_latency[i]);
    }
    printf("\n");
    printf("The average latency_of_mem_read:");
    for (int i = 0; i < num_dot; i++) {
        printf(" %lu", vec_ave_latency[i]);
    }
    printf("\n");
    printf("Please ignore this number:%lu\n", garbage_usage);

    // free variables
   
}

typedef struct Node {
    struct Node *next;


    #ifdef DEBUG
    int checkout;
    #endif
}Node;

// The following trick to unroll loop is from http://www.bitmover.com/lmbench/lat_mem_rd.8.html
#define	ONE	cur_node = cur_node->next;
#define	FIVE	ONE ONE ONE ONE ONE
#define	TEN	FIVE FIVE
#define	FIFTY	TEN TEN TEN TEN TEN
#define	HUNDRED	FIFTY FIFTY

uint64_t lat_mem(uint64_t size_LList) {

    // Step 1: init a linked list of size size_LList, with len_LList nodes
    Node *LList;
    int len_LList;
    LList = (Node *) malloc(size_LList);

    // Do we need this line?
    memset(LList, 0x00, size_LList);

    len_LList = size_LList / (sizeof(Node));


    #ifdef DEBUG
    if (LList != NULL) {
        printf("Succ to generate a linked list of %d nodes, whose size is %lu\n", len_LList, size_LList);
    } else {
        printf("Failed to generate a linked list, whose size is %lu\n", size_LList);
    }
    #endif

    // Link the nodes
    for (int i = 0; i < len_LList - 1; i++) {
        LList[i].next = &LList[i+1];

        #ifdef DEBUG
        LList[i].checkout = 2 * i;
        #endif
    }
    LList[len_LList - 1].next = NULL;

    #ifdef DEBUG
    if (size_LList == 512) {
        printf("See if linked list is OK. Now printing the .checkout of the 512 byte linked list:\n");
        Node *cur_node;
        cur_node = LList;
        while (cur_node != NULL) {
            printf("\t%d", cur_node->checkout);
            cur_node = cur_node->next;
        }
        printf("\n");
    }
    #endif

    // Step 2: measurement

    register int count_100 = (len_LList-1) / 100;
    register int count_rest = (len_LList-1) % 100;
    register int i;
    Node *cur_node;
    cur_node = LList;

    // variables for the cycle count
    uint64_t start, end, total = 0;
    unsigned start_low, start_high, end_low, end_high;

    // sfence and measurement
    _mm_sfence();
    asm volatile("CPUID\n\t"
                     "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     : "=r"(start_high), "=r"(start_low)::"%rax", "%rbx", "%rcx", "%rdx");

    
    for (i = 0; i < count_100; i++) {
        HUNDRED;
    }
    for (i = 0; i < count_rest; i++) {
        ONE;
    }
   

    

    // sfence and measurement
    _mm_sfence();
    asm volatile("RDTSCP\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     "CPUID\n\t"
                     : "=r"(end_high), "=r"(end_low)::"%rax", "%rbx", "%rcx", "%rdx");

    garbage_use_ptr(cur_node); // prevent compiler from optimize out our code

    start = (((uint64_t)start_high << 32) | start_low);
    end = (((uint64_t)end_high << 32) | end_low);
    total = end - start;

    total = total - count_100 * 100 - count_rest; // minus 1 cycle used to execute load instruction

    free(LList);

    return total;
}

void garbage_use_ptr(void *result) { garbage_usage += (uint64_t)result; }


