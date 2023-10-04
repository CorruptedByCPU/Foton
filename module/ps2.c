/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

__attribute__(( no_caller_saved_registers ))
void driver_ps2_mouse( void ) {
}

__attribute__(( no_caller_saved_registers ))
void driver_ps2_keyboard( void ) {
}

void _entry( uintptr_t kernel_ptr ) {
	// hold the door
	while( TRUE );
}
