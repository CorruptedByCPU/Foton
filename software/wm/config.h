/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	// hint
	// all operations are performed on pixels

	#define	WM_DEBUG_STARVATION_limit	1

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../../library/color.h"
	#include	"../../library/font.h"
	#include	"../../library/image.h"
	#include	"../../library/integer.h"
	#include	"../../library/interface.h"
	#include	"../../library/string.h"

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	#define	WM_OBJECT_LIMIT			64	// no more than 64 windows
	#define	WM_LIST_LIMIT			WM_OBJECT_LIMIT
	#define	WM_ZONE_LIMIT			(WM_OBJECT_LIMIT << STD_SHIFT_4)	// 4 edges per window
	#define	WM_TASKBAR_LIMIT		WM_OBJECT_LIMIT

	#define	WM_OBJECT_CURSOR_width		32
	#define	WM_OBJECT_CURSOR_height		32

	#define	WM_OBJECT_TASKBAR_HEIGHT_pixel	LIB_INTERFACE_HEADER_HEIGHT_pixel
	#define	WM_OBJECT_TASKBAR_ENTRY_pixel	(255 + 1)

	#define	WM_OBJECT_TASKBAR_CLOCK_pixel	50

	#define	WM_TASKBAR_BG_default		0xE8000000
	#define	WM_TASKBAR_BG_visible		0xFF101010
	#define	WM_TASKBAR_BG_invisible		0xFF000000
	#define	WM_TASKBAR_BG_active		0xFF181818

	struct	WM_STRUCTURE_OBJECT {
		int16_t		x;
		int16_t		y;
		int16_t		width;
		int16_t		height;
		int64_t		pid;
		uint64_t	size_byte;
		struct STD_WINDOW_STRUCTURE_DESCRIPTOR	*descriptor;
	};

	struct	WM_STRUCTURE_ZONE {
		int16_t		x;
		int16_t		y;
		int16_t		width;
		int16_t		height;
		struct WM_STRUCTURE_OBJECT *object;
	};

	void wm_event( void );
	void wm_sync( void );
	void wm_zone_insert( struct WM_STRUCTURE_ZONE *zone, uint8_t object );
	void wm_zone( void );
	void wm_fill( void );
	struct WM_STRUCTURE_OBJECT *wm_object_create( int16_t x, int16_t y, uint16_t width, uint16_t height );
	struct WM_STRUCTURE_OBJECT *wm_object_find( uint16_t x, uint16_t y, uint8_t parse_hidden );
	uint8_t wm_object_move_up( struct WM_STRUCTURE_OBJECT *object );
	void wm_object_move( int16_t x, int16_t y );
	void wm_object_remove( uint16_t i );
	void wm_object_active_new( void );
	int64_t wm_menu( void );