/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t kernel_time_rdtsc( void ) {
	// receive current CPU cycle count
	uint64_t rdtsc; __asm__ volatile( "push %%rdx\nrdtsc\nshl $32, %%rdx\nor %%rdx, %%rax\nmov %%rax, %0\npop %%rdx" : "=a" (rdtsc) );

	// return RDTSC value
	return rdtsc;
}

void kernel_time_sleep( uint64_t t ) {
	// set release pointer
	uint64_t stop = kernel -> time_rtc + t;

	// release CPU time until we achieve pointer
	while( stop > kernel -> time_rtc ) __asm__ volatile( "int $0x40" );
}
