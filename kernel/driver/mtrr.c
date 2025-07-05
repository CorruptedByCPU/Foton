/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t driver_mtrr_read( uint32_t msr ) {
	// registers
	uint32_t eax, edx = EMPTY;

	// read content of MSR address
	__asm__ volatile( "rdmsr" : "=a" (eax), "=d" (edx) : "c" (msr) );

	// return combined
	return ((uint64_t) edx << 32) | eax;
}

void driver_mtrr_write( uint32_t msr, uint64_t value ) {
	// registers
	uint32_t eax = (uint32_t) value & STD_MASK_dword;
	uint32_t edx = (uint32_t) (value >> 32) & STD_MASK_dword;
    
	// write value at MSR address
	__asm__ volatile ("wrmsr" :: "c" (msr), "a" (eax), "d" (edx) );
}
