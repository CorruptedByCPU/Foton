/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#include	"../library/window.h"

#define	WIDTH	320
#define	HEIGHT	200

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	struct STD_STRUCTURE_WINDOW_DESCRIPTOR *window_a = (struct STD_STRUCTURE_WINDOW_DESCRIPTOR *) lib_window( -1, -1, WIDTH, HEIGHT );
	struct STD_STRUCTURE_WINDOW_DESCRIPTOR *window_b = (struct STD_STRUCTURE_WINDOW_DESCRIPTOR *) lib_window( -1, -1, WIDTH, HEIGHT );
	struct STD_STRUCTURE_WINDOW_DESCRIPTOR *window_c = (struct STD_STRUCTURE_WINDOW_DESCRIPTOR *) lib_window( -1, -1, WIDTH, HEIGHT );

	uint32_t *pixel_a = (uint32_t *) ((uintptr_t) window_a + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ));
	for( uint16_t y = 0; y < HEIGHT; y++ ) for( uint16_t x = 0; x < WIDTH; x++ ) pixel_a[ (y * WIDTH) + x ] = 0xFFFF0000;

	uint32_t *pixel_b = (uint32_t *) ((uintptr_t) window_b + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ));
	for( uint16_t y = 0; y < HEIGHT; y++ ) for( uint16_t x = 0; x < WIDTH; x++ ) pixel_b[ (y * WIDTH) + x ] = 0xFF00FF00;

	uint32_t *pixel_c = (uint32_t *) ((uintptr_t) window_c + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ));
	for( uint16_t y = 0; y < HEIGHT; y++ ) for( uint16_t x = 0; x < WIDTH; x++ ) pixel_c[ (y * WIDTH) + x ] = 0xFF0000FF;

	window_a -> header_width = WIDTH;
	window_a -> header_height = HEIGHT;
	window_b -> header_width = WIDTH;
	window_b -> header_height = HEIGHT;
	window_c -> header_width = WIDTH;
	window_c -> header_height = HEIGHT;

	window_a -> flags = STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_resizable | STD_WINDOW_FLAG_flush;
	window_b -> flags = STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_resizable | STD_WINDOW_FLAG_flush;
	window_c -> flags = STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_resizable | STD_WINDOW_FLAG_flush;

	while( TRUE ) {
		struct STD_STRUCTURE_WINDOW_DESCRIPTOR *window_a_new = lib_window_event( window_a );
		if( window_a_new ) window_a = window_a_new;

		struct STD_STRUCTURE_WINDOW_DESCRIPTOR *window_b_new = lib_window_event( window_b );
		if( window_b_new ) window_b = window_b_new;

		struct STD_STRUCTURE_WINDOW_DESCRIPTOR *window_c_new = lib_window_event( window_c );
		if( window_c_new ) window_c = window_c_new;

		sleep( TRUE );
	}

	return 0;
}
