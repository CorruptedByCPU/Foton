/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	KERNEL_INIT_MODULE_count	5

uint8_t *module_list[ KERNEL_INIT_MODULE_count ] = {
	(uint8_t *) "shredder.ko",
	(uint8_t *) "ps2.ko",
	(uint8_t *) "usb.ko",
	(uint8_t *) "e1000.ko",
	(uint8_t *) "sb16.ko"
};

void kernel_init_module( void ) {
	// for every required module
	for( uint64_t i = 0; i < KERNEL_INIT_MODULE_count; i++ )
		// load it
		kernel_module_load( module_list[ i ], lib_string_length( module_list[ i ] ) );
}
