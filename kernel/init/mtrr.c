/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_mtrr( void ) {
	MACRO_DEBUF();
	for( uint8_t i = 0; i < 16; i += 2 ) {
		volatile uint64_t test = driver_mtrr_read( 0x200 + i );
		volatile uint64_t test1 = driver_mtrr_read( 0x201 + i );

		// // Set MTRR PHYSBASE0 for 0xC0000000 with Write-Combining type
		uint64_t phys_base = ((uint64_t) kernel -> framebuffer_base_address & ~KERNEL_MEMORY_mirror);
		if( phys_base == test ) {
		driver_mtrr_write(0x200 + i, phys_base | MTRR_TYPE_WC);
			
		volatile uint64_t phys_mask = ~(uint32_t)(0x800000 - 1) | ((uint64_t)1 << 11);
		driver_mtrr_write(0x201 + i, test1);
		}
	}
}
