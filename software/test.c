/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	#include	"../library/ui.h"

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	struct LIB_WINDOW_STRUCTURE *window = lib_window( 0, 0, 320, 200 );

	for( uint64_t y = 0; y < window -> height; y++ )
		for( uint64_t x = 0; x < window -> width; x++ )
			window -> pixel[ (y * window -> width) + x ] = STD_COLOR_GREEN;

	window -> flags = LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;

	// main loop
	while( TRUE );

	// end of execution
	return 0;
}
