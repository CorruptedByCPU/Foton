/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	WM_OBJECT_LIMIT			64	// no more than 64 windows
#define	WM_LIST_LIMIT			WM_OBJECT_LIMIT
#define	WM_ZONE_LIMIT			(WM_OBJECT_LIMIT << STD_SHIFT_4)	// 4 edges per window

struct	WM_STRUCTURE_OBJECT {
	int16_t		x;
	int16_t		y;
	int16_t		width;
	int16_t		height;
	int64_t		pid;
	uint64_t	size_byte;
	struct LIB_WINDOW_DESCRIPTOR	*descriptor;
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
