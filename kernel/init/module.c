/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	KERNEL_INIT_MODULE_count	3

uint8_t *module_list[ KERNEL_INIT_MODULE_count ] = {
	(uint8_t *) "ps2.ko",
	(uint8_t *) "usb.ko",
	(uint8_t *) "shredder.ko"
};

void kernel_init_module( void ) {
	// alloc memory map space for modules
	kernel -> module_map_address = (uint32_t *) kernel_memory_alloc( MACRO_PAGE_ALIGN_UP( (kernel -> page_limit >> STD_SHIFT_8) + 1 ) >> STD_SHIFT_PAGE );

	// initialize memory map
	kernel_memory_dispose( kernel -> module_map_address, EMPTY, kernel -> page_limit );

	// for every required module
	for( uint64_t i = 0; i < KERNEL_INIT_MODULE_count; i++ )
		// load it
		kernel_module_load( module_list[ i ], lib_string_length( module_list[ i ] ) );
}
