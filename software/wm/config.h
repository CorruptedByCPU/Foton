/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	// hint
	// all operations are performed on pixels

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../../library/color.h"

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	#define	WM_OBJECT_FLAG_flush	0b0000000000000001
	#define	WM_OBJECT_FLAG_visible	0b0000000000000010
	#define	WM_OBJECT_FLAG_fixed_xy	0b0000000000000100
	#define	WM_OBJECT_FLAG_fixed_z	0b0000000000001000

	#define	WM_OBJECT_CURSOR_width	32
	#define	WM_OBJECT_CURSOR_height	32

	struct	WM_STRUCTURE_OBJECT {
		int16_t		x;
		int16_t		y;
		uint16_t	width;
		uint16_t	height;
		int64_t		pid;
		uint64_t	size_byte;
		struct WM_STRUCTURE_DESCRIPTOR	*descriptor;
	};

	struct	WM_STRUCTURE_ZONE {
		int16_t		x;
		int16_t		y;
		uint16_t	width;
		uint16_t	height;
		struct WM_STRUCTURE_OBJECT *object;
	};

	// structure must be divisible by 0x10
	struct	WM_STRUCTURE_DESCRIPTOR {
		uint16_t	flags;
		uint16_t	width;
		uint16_t	height;
		uint16_t	reserved[ 5 ];
	} __attribute__( (packed) );

	struct	WM_STRUCTURE_REQUEST {
		uint16_t	width;
		uint16_t	height;
	} __attribute__( (packed) );

	struct	WM_STRUCTURE_ANSWER {
		uintptr_t	descriptor;
	} __attribute__( (packed) );

	void wm_event( void );
	void wm_sync( void );
	void wm_zone_insert( struct WM_STRUCTURE_ZONE *zone, uint8_t object );
	void wm_zone( void );
	void wm_fill( void );
	struct WM_STRUCTURE_OBJECT *wm_object_create( int16_t x, int16_t y, uint16_t width, uint16_t height );