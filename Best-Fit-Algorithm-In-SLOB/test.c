#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include "memory_wrapper.h"

#define counter_loop 5

int main(int argc, char *argv[]) {
	long total_alloc_mem=0, alloc_mem=0;
	long total_free_mem=0, free_mem=0;
	int i=0;

	while (i < counter_loop) {
		alloc_mem = slob_get_total_alloc_mem();
		free_mem = slob_get_total_free_mem();
		
		total_alloc_mem = total_alloc_mem + alloc_mem;
		total_free_mem = total_free_mem + free_mem;
		
		printf("Alloc memory: %ld\tFree memory: %ld\n", alloc_mem, free_mem);
		getchar();
		i++;
	}
	printf("\nTotal Alloc Memory: %ld\nTotal Free Memory: %ld\n", total_alloc_mem, total_free_mem);

	printf("\nAvg Alloc Memory: %ld\nAvg Free Memory: %ld\n", total_alloc_mem/counter_loop, total_free_mem/counter_loop);

	return (0);
}

