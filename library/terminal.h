/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_TERMINAL
	#define	LIB_TERMINAL

	#ifndef	LIB_FONT
		#include	"./font.h"
	#endif
	#ifndef	LIB_STRING
		#include	"./string.h"
	#endif

	#define LIB_TERMINAL_TAB_length		8	// chars

	#define	LIB_TERMINAL_FLAG_clean		1	// or 0b00000001, clean space for character under cursor

	struct LIB_TERMINAL_STRUCTURE {
		uint16_t	width;
		uint16_t	height;
		uint32_t	*base_address;
		uint32_t	scanline_pixel;
		uint32_t	scanline_line;
		uint32_t	*pointer;
		uint32_t	color_foreground;
		uint32_t	color_background;
		uint8_t		alpha;
		uint8_t		flags;
		uint16_t	width_char;
		uint16_t	height_char;
		uint16_t	scanline_char;
		uint16_t	cursor_x;
		uint16_t	cursor_y;
		uint16_t	cursor_x_preserved;
		uint16_t	cursor_y_preserved;
		uint64_t	cursor_lock;
	};

	// initialize terminal properties
	void lib_terminal( struct LIB_TERMINAL_STRUCTURE *terminal );

	// draw character on terminal
	void lib_terminal_char( struct LIB_TERMINAL_STRUCTURE *terminal, uint8_t ascii );

	// clear character space with default background color
	void lib_terminal_char_drain( struct LIB_TERMINAL_STRUCTURE *terminal );

	// hide, show and change cursor position
	void lib_terminal_cursor_disable( struct LIB_TERMINAL_STRUCTURE *terminal );
	void lib_terminal_cursor_enable( struct LIB_TERMINAL_STRUCTURE *terminal );
	void lib_terminal_cursor_set( struct LIB_TERMINAL_STRUCTURE *terminal );
	void lib_terminal_cursor_switch( struct LIB_TERMINAL_STRUCTURE *terminal );

	// clear whole terminal space
	void lib_terminal_drain( struct LIB_TERMINAL_STRUCTURE *terminal );

	// clear current line of terminal
	void lib_terminal_drain_line( struct LIB_TERMINAL_STRUCTURE *terminal );

	// clear selected line of terminal
	void lib_terminal_drain_line_n( struct LIB_TERMINAL_STRUCTURE *terminal, uint64_t n );

	// main printing function
	void lib_terminal_printf( struct LIB_TERMINAL_STRUCTURE *terminal, uint8_t *string, ... );

	// reinitialize standard values
	void lib_terminal_reload( struct LIB_TERMINAL_STRUCTURE *terminal );

	// move all lines up of terminal by one line
	void lib_terminal_scroll_up( struct LIB_TERMINAL_STRUCTURE *terminal );

	// move all lines down of terminal by one line
	void lib_terminal_scroll_down( struct LIB_TERMINAL_STRUCTURE *terminal );

	// draw whole string inside terminal
	void lib_terminal_string( struct LIB_TERMINAL_STRUCTURE *terminal, uint8_t *string, uint64_t length );

	// convert value to string (regarded of base 2, 8, 10, 16 etc.) and draw on screen
	void lib_terminal_value( struct LIB_TERMINAL_STRUCTURE *terminal, uint64_t value, uint8_t base, uint8_t prefix, uint8_t character );
#endif
