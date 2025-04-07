/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_TIME
	#define	KERNEL_TIME

	// // returns current cycle count of AP
	uint64_t kernel_time_rdtsc( void );

	// releases CPU after ~t miliseconds
	void kernel_time_sleep( uint64_t t );
#endif
