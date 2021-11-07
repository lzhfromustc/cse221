***Memory Operations***

**From instructor**
1. RAM access time: Report latency for individual integer accesses to main memory and the L1 and L2 caches. Present results as a graph with the x-axis as the log of the size of the memory region accessed, and the y-axis as the average latency. Note that the lmbench paper is a good reference for this experiment. In terms of the lmbench paper, measure the "back-to-back-load" latency and report your results in a graph similar to Fig. 1 in the paper. You should not need to use information about the machine or the size of the L1, L2, etc., caches when implementing the experiment; the experiment will reveal these sizes. In your graph, label the places that indicate the different hardware regimes (L1 to L2 transition, etc.).

2. RAM bandwidth: Report bandwidth for both reading and writing. Use loop unrolling to get more accurate results, and keep in mind the effects of cache line prefetching (e.g., see the lmbench paper).

3. Page fault service time: Report the time for faulting an entire page from disk (mmap is one useful mechanism). Dividing by the size of a page, how does it compare to the latency of accessing a byte from main memory?

Important reference: 
[1] lmbench: Portable Tools for Performance Analysis https://www.usenix.org/legacy/publications/library/proceedings/sd96/full_papers/mcvoy.pdf
[2] lmbench code manual: http://www.bitmover.com/lmbench


**RAM access time**
RAM access time is as known as memory read latency measurements. There are multiple definitions of memory read latency definitions: "the most common, in increasing time order, are memory chip cycle time, processor-pins-to-memory-and-back time, load-in-a-vacuum time, and back-to-back-load time" [1]. However, many definitions are based on hardware and hard to measure from software. Thus, according to CSE221's instruction, we are measuring the back-to-back-load time. Our algorithm and mechanism is adopted from [1].

To measure it, we create a linked list of XXX nodes, in which each node contains no data, just the pointer to the next node. Since we are using 64-bit machine, the size of each node is 8-byte. We use a loop to access every node in this linked list, and measure the CPU cycles cost. The number of instructions of each iteration is XXX, costing XXX cycles. We will show that the latency of memory load is much higher than the execution time, meaning the cost is dominated by the memory load time.

There are a few details:
1. We use memory barrier to guarantee the accuracy of our measurement, i.e., `_mm_sfence` in `xmmintrin.h`.
2. We use increasing size of linked list to draw a figure of latency-size figure. We will see that this figure has four platforms, representing three levels of cache and the main memory.
3. One CPU cycle is needed to do the load instruction. So zero latency is defined as one CPU cycle.
4. (TODO: do we need this?) Use multiple strides to see the min latency.

**Read bandwidth**
Read bandwidth is the speed of data movement of the main memory.

To measure it, we create an array of int (4 bytes in our machine), and measure the time of adding these int up, using a manually unrolled loop. We take the following steps:
1. Create int array. This array is declared as a static int array. Otherwise the stack is not enough and will seg fault.
2. Create two char arrays with a size larger than our cache, and use memcpy to copy one to another. This can fill the cache with garbage, so we can measure the main memory in the next steps. Also, these arrays are declared as global variables to avoid seg fault.
3. Measure the execution CPU cycles of the unrolled loop. We unroll it 40000 times to minimize the overhead of loop.
4. Repeat the steps 10 times to measure average time.

**Page fault**
Page fault is measured by the following steps:
1. Create a file in disk.
2. Flush page cache. (TODO: how to do this? madvise?)
3. Map the file to memory.
4. Disable page prefetch. (TODO: how to do this? madvise?)
5. Fill cache with garbage.
6. In each iteration of a loop, access just 1 byte of the file. Each iteration jumps a large step, to make sure the byte accessed is not in the same page. In this way, the number of iteration is the number of page fault.
7. Repeat the steps 10 times to measure average time.