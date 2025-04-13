/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	KERNEL_INIT_MODULE_count	2

uint8_t *module_list[ KERNEL_INIT_MODULE_count ] = {
	(uint8_t *) "idle.ko",
	(uint8_t *) "usb.ko"
};

void kernel_init_module( void ) {
	// for every required module
	for( uint64_t i = INIT; i < KERNEL_INIT_MODULE_count; i++ )
		// load
		kernel_module( module_list[ i ], lib_string_length( module_list[ i ] ) );
}
