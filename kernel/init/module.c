/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t *module_list[] = {
	(uint8_t *) "idle.ko",
	(uint8_t *) "shredder.ko",
	(uint8_t *) "ps2.ko",
	(uint8_t *) "virtio.ko",
	EMPTY
};

void kernel_init_module( void ) {
	// for every required module
	for( uint64_t i = 0; module_list[ i ]; i++ )
		// load
		kernel_module( module_list[ i ], lib_string_length( module_list[ i ] ) );
}
