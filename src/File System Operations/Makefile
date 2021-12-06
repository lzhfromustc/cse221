all: cache_size sequential_read random_read file_read contention


cache_size: cache_size.c
	gcc cache_size.c -o build/cache_size1
sequential_read: sequential_read.c
	gcc sequential_read.c -o build/sequential_read
random_read: random_read.c
	gcc random_read.c -o build/random_read
file_read: file_read.c
	gcc file_read.c -o build/file_read
contention: contention.c
	gcc contention.c -o build/contention


clean:
	if [ -d build ];then \
		rm -r build; \
	fi
