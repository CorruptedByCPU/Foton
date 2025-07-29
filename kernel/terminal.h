/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_TERMINAL
	#define KERNEL_TERMINAL

	// matrix dimension
	#define LIB_FONT_MATRIX_width_pixel		1481
	#define LIB_FONT_MATRIX_height_pixel		16

	struct KERNEL_STRUCTURE_TERMINAL {
		// semaphore, exclusive access
		uint8_t		lock;

		// terminal dimension in characters
		uint16_t	width_char;
		uint16_t	height_char;

		// normal scanline but with height of character
		uint32_t	scanline_pixel;

		// cursor position inside terminal environment
		uint16_t	cursor_x;
		uint16_t	cursor_y;

		// first pixel of terminal
		uint32_t 	*pixel;
		// cursor position inside memory
		uint32_t	*pointer;

		// cache of all printed characters
		uint8_t		*string;
		uint64_t	length;

		// visualization
		uint32_t	color_foreground;
		uint32_t	color_background;
	};

	static void kernel_terminal_clean_character( void );
	static void kernel_terminal_cursor( void );
	void kernel_terminal_printf( const char *string, ... );
	void kernel_terminal_register( char character );
	static void kernel_terminal_scroll( void );
	void kernel_terminal_value( uint64_t value, uint8_t base, uint8_t prefix, uint8_t character );
#endif
