/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_log( uint8_t *string, ... ) {
	// properties of argument list
	va_list argv;

	// start of argument list
	va_start( argv, string );

	#ifdef	DEBUG
		// parse string with arguments
		lib_terminal_parse( &kernel_terminal, string, argv );
	#endif

	// end of arguemnt list
	va_end( argv );
}