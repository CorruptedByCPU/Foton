/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void _entry( void ) {
	// infinite loop :)
	while( TRUE ) __asm__ volatile( "hlt" );
}
