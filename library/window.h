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
	
	#define	LIB_WINDOW_FLAG_active		(1 << 0)	// window will receive mouse/keyboard events
	#define	LIB_WINDOW_FLAG_visible		(1 << 1)	// window is visible on workbench
	#define	LIB_WINDOW_FLAG_fixed_xy	(1 << 2)	// window unmovable
	#define	LIB_WINDOW_FLAG_fixed_z		(1 << 3)	// window cannot be dragged up
	#define	LIB_WINDOW_FLAG_release		(1 << 4)	// window marked as ready to be removed
	#define	LIB_WINDOW_FLAG_name		(1 << 5)
	#define	LIB_WINDOW_FLAG_transparent	(1 << 6)	// window contains transparent pixels (high CPU usage...)
	#define	LIB_WINDOW_FLAG_icon		(1 << 7)	// icon exist
	#define	LIB_WINDOW_FLAG_resizable	(1 << 8)
	#define	LIB_WINDOW_FLAG_properties	(1 << 9)	// Window Manager proposed new window properties
	#define	LIB_WINDOW_FLAG_enlarge		(1 << 10)
	#define	LIB_WINDOW_FLAG_menu		(1 << 11)
	#define	LIB_WINDOW_FLAG_flush		(1 << 12)
	#define	LIB_WINDOW_FLAG_panel		(1 << 13)
	#define	LIB_WINDOW_FLAG_cursor		(1 << 14)
	#define	LIB_WINDOW_FLAG_hide		(1 << 15)

	#define	LIB_WINDOW_REQUEST_create	0b00000001

	#define	LIB_WINDOW_ANSWER_create	0b10000000 | LIB_WINDOW_REQUEST_create

	#define	LIB_WINDOW_ICON_size		16

	struct	LIB_WINDOW_STRUCTURE {
		volatile uint16_t	flags;
		// base address of window content
		uint32_t	*pixel;
		// pointer position inside window
		uint16_t	x;
		uint16_t	y;
		int16_t		z;
		// assigned window dimension
		uint16_t	current_x;
		uint16_t	current_y;
		uint16_t	current_width;
		uint16_t	current_height;
		// minimal allowed window dimension
		uint16_t	width_minimal;
		uint16_t	height_minimal;
		// proposed window properties
		int16_t		new_x;
		int16_t		new_y;
		uint16_t	new_width;
		uint16_t	new_height;
		// preserved window properties
		int16_t		old_x;
		int16_t		old_y;
		uint16_t	old_width;
		uint16_t	old_height;
		// pointer position regardless of window
		uint16_t	absolute_x;
		uint16_t	absolute_y;
		// header properties, used by window movement
		uint8_t		header_offset;
		uint8_t		header_height;
		uint16_t	header_width;
		// window name and icon, it will appear at header/panel
		uint8_t		name_length;
		uint8_t		name[ 32 + TRUE ];
		uint32_t	icon[ 16 * 16 ];
	} __attribute__( (aligned( STD_PAGE_byte )) );

	struct LIB_WINDOW_LIST {
		uint64_t	id;
		uint16_t	flags;
		uint8_t		name_limit;
		uint8_t		name[ 32 + TRUE ];
	};

	struct LIB_WINDOW_STRUCTURE *lib_window( int16_t x, int16_t y, uint16_t width, uint16_t height );

	struct LIB_WINDOW_STRUCTURE *lib_window_event( struct LIB_WINDOW_STRUCTURE *descriptor );

	void lib_window_name( struct LIB_WINDOW_STRUCTURE *descriptor, uint8_t *name );
#endif
