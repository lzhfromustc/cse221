cd Memory\ Operations
gcc -O0 latency.c pagefault.c bandwidth.c main.c -o ./mem_main
sudo nice -n -20 ./mem_main