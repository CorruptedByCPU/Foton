/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef LIB_WINDOW
	#define	LIB_WINDOW

	#define	LIB_WINDOW_BORDER_pixel				1
	#define	LIB_WINDOW_BORDER_COLOR_default			0xFF282828
	#define	LIB_WINDOW_BORDER_COLOR_default_shadow		0xFF202020
	#define	LIB_WINDOW_BORDER_COLOR_inactive		LIB_WINDOW_BORDER_COLOR_default_shadow
	#define	LIB_WINDOW_BORDER_COLOR_inactive_shadow		0xFF181818
	
	#define	LIB_WINDOW_FLAG_active		(1 << 0)
	#define	LIB_WINDOW_FLAG_visible		(1 << 1)
	#define	LIB_WINDOW_FLAG_fixed_xy	(1 << 2)
	#define	LIB_WINDOW_FLAG_fixed_z		(1 << 3)
	#define	LIB_WINDOW_FLAG_release		(1 << 4)	// window marked as ready to be removed
	#define	LIB_WINDOW_FLAG_name		(1 << 5)
	#define	LIB_WINDOW_FLAG_minimize	(1 << 6)
	#define	LIB_WINDOW_FLAG_unstable	(1 << 7)	// hide window on any mouse button press
	#define	LIB_WINDOW_FLAG_resizable	(1 << 8)
	#define	LIB_WINDOW_FLAG_properties	(1 << 9)	// Window Manager proposed new window properties
	#define	LIB_WINDOW_FLAG_maximize	(1 << 10)
	#define	LIB_WINDOW_FLAG_flush		(1 << 12)
	#define	LIB_WINDOW_FLAG_panel		(1 << 13)
	#define	LIB_WINDOW_FLAG_cursor		(1 << 14)
	#define	LIB_WINDOW_FLAG_hide		(1 << 15)

	#define	LIB_WINDOW_REQUEST_create	0b00000001

	#define	LIB_WINDOW_ANSWER_create	0b10000000 | LIB_WINDOW_REQUEST_create

	struct	LIB_WINDOW_STRUCTURE_DESCRIPTOR {
		uint32_t	flags;
		// pointer position inside window
		uint16_t	x;
		uint16_t	y;
		uint16_t	z;
		// assigned window dimension
		uint16_t	width;
		uint16_t	height;
		// minimal dimension of window allowed
		uint16_t	width_minimal;
		uint16_t	height_minimal;
		// proposed window properties
		int16_t		new_x;
		int16_t		new_y;
		uint16_t	new_width;
		uint16_t	new_height;
		// header properties, used by window movement
		uint8_t		header_offset;
		uint8_t		header_height;
		uint16_t	header_width;
		// window name, it will appear at header/panel
		uint8_t		name_length;
		uint8_t		name[ 32 + TRUE ];
	} __attribute__( (aligned( STD_PAGE_byte )) );

	struct LIB_WINDOW_LIST {
		uint64_t	id;
		uint16_t	flags;
		uint8_t		name_limit;
		uint8_t		name[ 32 + TRUE ];
	};

	struct LIB_WINDOW_STRUCTURE_DESCRIPTOR *lib_window( int16_t x, int16_t y, uint16_t width, uint16_t height );

	struct LIB_WINDOW_STRUCTURE_DESCRIPTOR *lib_window_event( struct LIB_WINDOW_STRUCTURE_DESCRIPTOR *descriptor );

	void lib_window_name( struct LIB_WINDOW_STRUCTURE_DESCRIPTOR *descriptor, uint8_t *name );
#endif
