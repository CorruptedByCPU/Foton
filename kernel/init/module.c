/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	KERNEL_INIT_MODULE_count	1

uint8_t *module_list[ KERNEL_INIT_MODULE_count ] = {
	(uint8_t *) "usb.ko"
};

void kernel_init_module( void ) {
	// alloc memory map space for modules
	kernel -> module_base_address = (uint32_t *) kernel_memory_alloc( KERNEL_MODULE_MEMORY_MAP_page );

	// mark whole memory map as available for use
	kernel_memory_dispose( kernel -> module_base_address, EMPTY, (KERNEL_MODULE_MEMORY_MAP_page << STD_SHIFT_PAGE) << STD_SHIFT_8 );

	// for every required module
	for( uint64_t i = 0; i < KERNEL_INIT_MODULE_count; i++ )
		// load it
		kernel_module_load( module_list[ i ], lib_string_length( module_list[ i ] ) );
}
