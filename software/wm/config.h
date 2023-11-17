/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	// hint
	// all operations are performed on pixels

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../../library/color.h"
	#include	"../../library/font.h"
	#include	"../../library/image.h"

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	#define	WM_OBJECT_CURSOR_width		32
	#define	WM_OBJECT_CURSOR_height		32

	#define	WM_OBJECT_TASKBAR_HEIGHT_pixel	22
	#define	WM_OBJECT_TASKBAR_ENTRY_pixel	(255 + 1)

	#define	WM_TASKBAR_BG_default	0xA0000000
	#define	WM_TASKBAR_BG_invisible	0xE0000000
	#define	WM_TASKBAR_BG_visible	0xC0101010

	struct	WM_STRUCTURE_OBJECT {
		int16_t		x;
		int16_t		y;
		uint16_t	width;
		uint16_t	height;
		int64_t		pid;
		uint64_t	size_byte;
		struct STD_WINDOW_STRUCTURE_DESCRIPTOR	*descriptor;
	};

	struct	WM_STRUCTURE_ZONE {
		int16_t		x;
		int16_t		y;
		uint16_t	width;
		uint16_t	height;
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
	void wm_object_remove( struct WM_STRUCTURE_OBJECT *object );