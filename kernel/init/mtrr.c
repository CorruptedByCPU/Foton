/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_mtrr( void ) {
	// amount of variable-sized registers
	uint32_t ecx = driver_mtrr_read( 0x000000FE );

	// Write-Combining type is available?
	if( ! (ecx & (1 << 11)) ) return;	// no :(

	// search for framebuffer entry
	for( uint8_t i = 0; i < (ecx & STD_MASK_byte); i++ ) {
		// physical framebuffer address
		uint64_t framebuffer_base_address = ((uint64_t) kernel -> framebuffer_base_address & ~KERNEL_MEMORY_mirror);
		if( framebuffer_base_address == driver_mtrr_read( 0x200 + (i << STD_SHIFT_2) ) ) {
			// add Write-Combining flag
			driver_mtrr_write( 0x200 + i, framebuffer_base_address | TRUE );
			
			// and size as original
			driver_mtrr_write( 0x201 + i, driver_mtrr_read( 0x201 + (i << STD_SHIFT_2) ) );
		}
	}
}
