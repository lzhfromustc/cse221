***Memory Operations***

**0. Algorithm design and explainations**
For more algorithm design and explainations, results and discussion, please see Section 4 of our paper submitted in Gradescope.

This document will only show how the code is organized.




**1. RAM access time**
RAM access time is as known as memory read latency measurements. The code to measure it is in `latency.c` and `latency.h`, and the entry function to the measurement is in `main.c`.

In `latency.c`, function `measure_mem_lat()` first computes a vector holding the sizes of each linked list. The first 10 linked lists grow in 2^N, and the next 8 grow with 100000*N, and the next 20 grow in 1000000*N.
Then it invokes function `lat_mem()` to measure the latency of reading nodes in each linked list.
Finally, it prints the results both to the command line and to a file `latency_result.json`

In `latency.c`, function `lat_mem()` allocs a linked list, initiates the nodes in the linked list, and then measure the CPU cycles cost to read nodes in the linked list.



**2. Read bandwidth**
Read bandwidth is the speed of data movement of the main memory. The code to measure it is in `bandwidth.c` and `bandwidth.h`, and the entry function to the measurement is in `main.c`.

In `bandwidth.c`, function `measure_bw()` creates an array of int, fills the cache with garbage, and then measure the cost of adding up the int in the array by an unrolled loop.
Finally, it prints the results to the command line.



**3. Page fault**
Page fault is the time of each page fault when reading a file in disk. The code to measure it is in `pagefault.c` and `pagefault.h`, and the entry function to the measurement is in `main.c`.

In `pagefault.c`, function `measure_page_fault()` creates a file, maps it to the memory, fills the cache with garbage, and then reads bytes in the file. Each byte we read is far from the last byte, to make sure they are in different pages.
Finally, it prints the results to the command line.
