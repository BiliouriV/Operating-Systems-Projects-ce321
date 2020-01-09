#include <sys/syscall.h>
#include <unistd.h>
#include "memory_wrapper.h"

long int slob_get_total_free_mem(void){
	return( syscall(__NR_slob_get_total_free_mem) );
}

long int slob_get_total_alloc_mem(void){
	return( syscall(__NR_slob_get_total_alloc_mem) );
}

