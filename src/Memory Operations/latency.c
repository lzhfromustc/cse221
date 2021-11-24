#include "latency.h"

typedef struct Node {
    struct Node *next;


    #ifdef DEBUG
    int checkout;
    #endif
} Node;


int main() {
    printf("=====Starting memory latency measurement\n");

    // parameters
    uint64_t num_dot = 15; // number of maximum data points in the figure; default: 15 points
    uint64_t min_size = 1<<8; // the minimal linked list size; default: 256 B
    uint64_t max_size = 64<<20; // the maximum linked list size; default: 16 MB
    
    measure_mem_lat(num_dot, min_size, max_size);

    printf("=====Finished memory latency measurement\n\n\n");
}

int measure_mem_lat(uint64_t num_dot, uint64_t min_size, uint64_t max_size) {

    printf("Size of Node:%ld\n\n", sizeof(Node));

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
    int cycle_amplify = 1000; // in case the result is 0.XXX, we multiply the result with cycle_amplify to preserve XXX
    for (int i = 0; i < num_dot; i++) {
        Result r = lat_mem(vec_size_LList[i]);
        vec_latency[i] = r.result;
        uint64_t s = r.size;
        if (vec_latency[i] > prev_latency) { // Normal
            // This is a little inaccurate: maybe the last few members in LList is not accessed
            vec_ave_latency[i] = cycle_amplify * (vec_latency[i] - prev_latency) / (s - prev_size); 
        } else {
            vec_ave_latency[i] = 0;
        }
        
        prev_latency = vec_latency[i];
        prev_size = s;
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
    printf("The average latency_of_mem_read (In 1/%d cycles):", cycle_amplify);
    for (int i = 0; i < num_dot; i++) {
        printf(" %lu", vec_ave_latency[i]);
    }
    printf("\n");
    printf("Please ignore this number:%lu\n", garbage_usage);

    // free variables
   
}



// The following trick to unroll loop is from http://www.bitmover.com/lmbench/lat_mem_rd.8.html
#define	ONE	cur_node = cur_node->next;
#define	FIVE	ONE ONE ONE ONE ONE
#define	TEN	FIVE FIVE
#define	FIFTY	TEN TEN TEN TEN TEN
#define	HUNDRED	FIFTY FIFTY

Result lat_mem(uint64_t size_LList) {

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
    // If we just let node[i] point to node[i+1], then when we access node[i+1] we will have a cache hit, meaning we are just measuring L1 cache
    // So we have a stride variable. node[i] points to node[i + (stride / sizeof(node)) ]. When stride is no less than 64 bytes, our measurement will be OK
    // However, if stride is a constant, the CPU may predict and prefetch the next cache line. Let's see what will happen.

    int stride = 64; // in byte
    int indexGap = stride / sizeof(Node); // stride / 8

    int next_index = indexGap;
    int last_index = 0;
    int num_Node = 0;
    while (next_index < len_LList - 1) {
        
        LList[last_index].next = &LList[next_index];

        #ifdef DEBUG
        LList[last_index].checkout = last_index;
        #endif

        last_index = next_index;
        next_index += indexGap;   
        num_Node ++;     
    }
    LList[next_index].next = NULL;

    #ifdef DEBUG
    if (size_LList == 256) {
        printf("See if linked list is OK. Now printing the .checkout of the 256 byte linked list:\n");
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

    register int count_100 = (num_Node-1) / 100;
    register int count_rest = (num_Node-1) % 100;
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

    // Note that each access "ONE" is loading a whole cache line, which is 64 bits
    for (i = 0; i < count_100; i++) {
        HUNDRED;
    }
    for (i = 0; i < count_rest; i++) {
        ONE;
    }
   

    

    // sfence and measurement
    
    asm volatile("RDTSCP\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     "CPUID\n\t"
                     : "=r"(end_high), "=r"(end_low)::"%rax", "%rbx", "%rcx", "%rdx");
    _mm_sfence();

    garbage_use_ptr(cur_node); // prevent compiler from optimize out our code

    start = (((uint64_t)start_high << 32) | start_low);
    end = (((uint64_t)end_high << 32) | end_low);
    total = end - start;

    total = total - num_Node * 1; // minus 1 cycle used to execute load instruction

    free(LList);

    Result r;
    r.result = total;
    r.size = num_Node * stride;

    return r;
}

void garbage_use_ptr(void *result) { garbage_usage += (uint64_t)result; }


