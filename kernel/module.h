/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_MODULE
	#define	KERNEL_MODULE

	#define	KERNEL_MODULE_base_address	0xFFFFFFFF90000000

	// loads pointed module
	void kernel_module_load( uint8_t *path, uint64_t length );
#endif