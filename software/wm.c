/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	// hint
	// all operations are performed on pixels

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/color.h"

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	#define	WM_OBJECT_FLAG_flush	0b0000000000000001
	#define	WM_OBJECT_FLAG_visible	0b0000000000000010
	#define	WM_OBJECT_FLAG_fixed_xy	0b0000000000000100
	#define	WM_OBJECT_FLAG_fixed_z	0b0000000000001000

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

	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	int64_t	wm_pid = EMPTY;

	struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER framebuffer;

	struct WM_STRUCTURE_OBJECT *wm_object_base_address = EMPTY;
	struct WM_STRUCTURE_OBJECT **wm_list_base_address = EMPTY;
	struct WM_STRUCTURE_ZONE *wm_zone_base_address = EMPTY;

	uint64_t wm_object_limit = EMPTY;
	uint64_t wm_list_limit = EMPTY;
	uint64_t wm_zone_limit = EMPTY;

	uint8_t wm_object_semaphore = FALSE;
	uint8_t wm_list_semaphore = FALSE;
	uint8_t	wm_framebuffer_semaphore = FALSE;

	struct WM_STRUCTURE_OBJECT *wm_object_workbench = EMPTY;
	struct WM_STRUCTURE_OBJECT *wm_object_cursor = EMPTY;
	struct WM_STRUCTURE_OBJECT *wm_object_active = EMPTY;

void wm_sync( void ) {
	// requested synchronization?
	if( ! wm_framebuffer_semaphore ) return;	// no

	// copy the contents of the buffer to the memory space of the graphics card
	uint32_t *source = (uint32_t *) ((uintptr_t) wm_object_workbench -> descriptor + sizeof( struct WM_STRUCTURE_DESCRIPTOR ));
	for( uint32_t i = 0; i < (framebuffer.width_pixel * framebuffer.height_pixel); i++ )
			framebuffer.base_address[ i ] = source[ i ];

	// request accepted
	wm_framebuffer_semaphore = FALSE;
}

void wm_fill( void ) {
	// fill every zone with assigned object
	for( uint64_t i = 0; i < wm_zone_limit; i++ ) {
		// object assigned to zone?
		if( ! wm_zone_base_address[ i ].object ) continue;	// no

		// fill zone with selected object
		uint32_t *source = (uint32_t *) ((uintptr_t) wm_zone_base_address[ i ].object -> descriptor + sizeof( struct WM_STRUCTURE_DESCRIPTOR ));
		uint32_t *target = (uint32_t *) ((uintptr_t) wm_object_workbench -> descriptor + sizeof( struct WM_STRUCTURE_DESCRIPTOR ));
		for( uint64_t y = wm_zone_base_address[ i ].y; y < wm_zone_base_address[ i ].height + wm_zone_base_address[ i ].y; y++ )
			for( uint64_t x = wm_zone_base_address[ i ].x; x < wm_zone_base_address[ i ].width + wm_zone_base_address[ i ].x; x++ ) {
				// color properties
				uint32_t color_current = target[ (y * wm_object_workbench -> width) + x ];
				uint32_t color_new = source[ (x - wm_zone_base_address[ i ].object -> x) + (wm_zone_base_address[ i ].object -> width * (y - wm_zone_base_address[ i ].object -> y)) ];

				// perform the operation based on the alpha channel
				switch( color_new >> 24 ) {
					// no alpha channel
					case (uint8_t) 0xFF: {
						target[ (y * wm_object_workbench -> width) + x ] = color_new; continue;
					}

					// transparent color
					case 0x00: { continue; }
				}

				// calculate the color based on the alpha channel
				target[ (y * wm_object_workbench -> width) + x ] = lib_color_blend( color_current, color_new );
			}

		// synchronize workbench with framebuffer
		wm_framebuffer_semaphore = TRUE;
	}

	// all zones filled
	wm_zone_limit = EMPTY;
}

void wm_zone_insert( struct WM_STRUCTURE_ZONE *zone, uint8_t object ) {
	// discard zone if outside of workbench area
	if( zone -> x > wm_object_workbench -> width - 1 ) return;
	if( zone -> y > wm_object_workbench -> height - 1 ) return;
	if( zone -> x + zone -> width < 0 ) return;
	if( zone -> y + zone -> height < 0 ) return;

	// expand zone list if needed
	wm_zone_base_address = (struct WM_STRUCTURE_ZONE *) realloc( wm_zone_base_address, sizeof( struct WM_STRUCTURE_ZONE ) * (wm_zone_limit + 1) );

	// inset zone

	// truncate X axis
	if( zone -> x < 0 ) {
		// left side
		wm_zone_base_address[ wm_zone_limit ].width = zone -> width - (~zone -> x + 1);
		wm_zone_base_address[ wm_zone_limit ].x = 0;
	} else if( zone -> x + zone -> width > wm_object_workbench -> width ) {
		// right side
		wm_zone_base_address[ wm_zone_limit ].x = zone -> x;
		wm_zone_base_address[ wm_zone_limit ].width = zone -> width - ((zone -> x + zone -> width) - (int16_t) wm_object_workbench -> width);
	} else {
		// whole zone
		wm_zone_base_address[ wm_zone_limit ].x = zone -> x;
		wm_zone_base_address[ wm_zone_limit ].width = zone -> width;
	}

	// truncate Y axis
	if( zone -> y < 0 ) {
		// up side
		wm_zone_base_address[ wm_zone_limit ].height = zone -> height - (~zone -> y + 1);
		wm_zone_base_address[ wm_zone_limit ].y = 0;
	} else if( zone -> y + zone -> height > wm_object_workbench -> height ) {
		// bottom side
		wm_zone_base_address[ wm_zone_limit ].y = zone -> y;
		wm_zone_base_address[ wm_zone_limit ].height = zone -> height - ((zone -> y + zone -> height) - (int16_t) wm_object_workbench -> height);
	} else {
		// whole zone
		wm_zone_base_address[ wm_zone_limit ].y = zone -> y;
		wm_zone_base_address[ wm_zone_limit ].height = zone -> height;
	}

	// object selected for zone?
	if( object ) wm_zone_base_address[ wm_zone_limit ].object = zone -> object;
	else	// no
		wm_zone_base_address[ wm_zone_limit ].object = EMPTY;


	// zone inserted
	wm_zone_limit++;
}

void wm_zone( void ) {
	// parse zones on list
	for( uint64_t i = 0; i < wm_zone_limit; i++ ) {
		// object assigned to zone?
		if( wm_zone_base_address[ i ].object ) continue;	// yes

		// analyze zone against each object
		for( uint64_t j = 0; j < wm_list_limit; j++ ) {
			// invisible object?
			if( ! (wm_list_base_address[ j ] -> descriptor -> flags & WM_OBJECT_FLAG_visible) ) continue;	// yes

			// zone and object share area?
			if( wm_list_base_address[ j ] -> x + wm_list_base_address[ j ] -> width < wm_zone_base_address[ i ].x ) continue;	// no
			if( wm_list_base_address[ j ] -> y + wm_list_base_address[ j ] -> height < wm_zone_base_address[ i ].y ) continue;	// no
			if( wm_list_base_address[ j ] -> x > wm_zone_base_address[ i ].x + wm_zone_base_address[ i ].width ) continue;	// no
			if( wm_list_base_address[ j ] -> y > wm_zone_base_address[ i ].y + wm_zone_base_address[ i ].height ) continue;	// no

			// modify zone up to object boundaries
			struct WM_STRUCTURE_ZONE zone = wm_zone_base_address[ i ];

			// left edge
			if( zone.x < wm_list_base_address[ j ] -> x ) {
				zone.width -= wm_list_base_address[ j ] -> x - zone.x;
				zone.x = wm_list_base_address[ j ] -> x;
			}

			// top edge
			if( zone.y < wm_list_base_address[ j ] -> y ) {
				zone.height -= wm_list_base_address[ j ] -> y - zone.y;
				zone.y = wm_list_base_address[ j ] -> y;
			}

			// right edge
			if( (zone.x + zone.width) > (wm_list_base_address[ j ] -> x + wm_list_base_address[ j ] -> width) ) {
				zone.width -= (zone.x + zone.width) - (wm_list_base_address[ j ] -> x + wm_list_base_address[ j ] -> width);
			}

			// bottom edge
			if( (zone.y + zone.height) > (wm_list_base_address[ j ] -> y + wm_list_base_address[ j ] -> height) ) {
				zone.height -= (zone.y + zone.height) - (wm_list_base_address[ j ] -> y + wm_list_base_address[ j ] -> height);
			}

			// fill the zone with the given object
			zone.object = wm_list_base_address[ j ];
			wm_zone_insert( (struct WM_STRUCTURE_ZONE *) &zone, TRUE );
		}
	}
}

void wm_object( void ) {
	// search whole list for object flush
	for( uint16_t i = 0; i < wm_list_limit; i++ ) {
		// object visible and requested flush?
		if( wm_list_base_address[ i ] -> descriptor -> flags & WM_OBJECT_FLAG_visible && wm_list_base_address[ i ] -> descriptor -> flags & WM_OBJECT_FLAG_flush ) {
			// parse whole object area
			wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm_list_base_address[ i ], FALSE );

			// request accepted
			wm_list_base_address[ i ] -> descriptor -> flags ^= WM_OBJECT_FLAG_flush;
		}
	}
}

void wm_object_insert( struct WM_STRUCTURE_OBJECT *object ) {
	// block modification of object list
	while( __sync_val_compare_and_swap( &wm_list_semaphore, UNLOCK, LOCK ) );

	// find available entry or extend object list
	while( TRUE ) {
		// search for empty list entry
		for( uint64_t i = 0; i < wm_list_limit; i++ ) {
			// entry in use?
			if( ! wm_list_base_address[ i ] ) {
				// insert object on list
				wm_list_base_address[ i ] = object;

				// release list for modification
				wm_list_semaphore = UNLOCK;

				// done
				return;
			}
		}

		// no available entry

		// extend object list
		wm_list_base_address = (struct WM_STRUCTURE_OBJECT **) realloc( wm_list_base_address, sizeof( struct WM_STRUCTURE_OBJECT * ) * ++wm_list_limit );
	}
}

struct WM_STRUCTURE_OBJECT *wm_object_create( int16_t x, int16_t y, uint16_t width, uint16_t height ) {
	// block modification of object table
	while( __sync_val_compare_and_swap( &wm_object_semaphore, UNLOCK, LOCK ) );

	// find available entry or extend object table
	while( TRUE ) {
		// search for empty entry inside table
		for( uint64_t i = 0; i < wm_object_limit; i++ ) {
			// entry in use?
			if( ! wm_object_base_address[ i ].descriptor ) {	// no
				// fill object properties
				wm_object_base_address[ i ].x = x;
				wm_object_base_address[ i ].y = y;
				wm_object_base_address[ i ].width = width;
				wm_object_base_address[ i ].height = height;

				// calculate object area size in Bytes
				wm_object_base_address[ i ].size_byte = (width * height * STD_VIDEO_DEPTH_byte) + sizeof( struct WM_STRUCTURE_DESCRIPTOR );

				// assign area for object
				wm_object_base_address[ i ].descriptor = (struct WM_STRUCTURE_DESCRIPTOR *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( wm_object_base_address[ i ].size_byte ) >> STD_SHIFT_PAGE );

				// fill descriptor with known values
				wm_object_base_address[ i ].descriptor -> width = width;
				wm_object_base_address[ i ].descriptor -> height = height;

				// register object on list
				wm_object_insert( (struct WM_STRUCTURE_OBJECT *) &wm_object_base_address[ i ] );

				// newly created object becomes active
				wm_object_active = (struct WM_STRUCTURE_OBJECT *) &wm_object_base_address[ i ];

				// release table for modification
				wm_object_semaphore = UNLOCK;

				// ready
				return wm_object_active;
			}
		}

		// no available entry

		// extend object table
		wm_object_base_address = (struct WM_STRUCTURE_OBJECT *) realloc( wm_object_base_address, sizeof( struct WM_STRUCTURE_OBJECT ) * ++wm_object_limit );
	}
}

void wm_init( void ) {
	// get our PID number
	wm_pid = std_pid();

	// obtain information about kernel framebuffer
	std_framebuffer( &framebuffer );

	// framebuffer locked?
	if( framebuffer.owner_pid != wm_pid ) { log( "WM: Framebuffer is already in use. Exit." ); while( TRUE ); }

	//----------------------------------------------------------------------

	// prepare space for an array of objects
	wm_object_base_address = (struct WM_STRUCTURE_OBJECT *) malloc( TRUE );

	// prepare space for an array of object list
	wm_list_base_address = (struct WM_STRUCTURE_OBJECT **) malloc( TRUE );

	// prepare space for an list of zones
	wm_zone_base_address = (struct WM_STRUCTURE_ZONE *) malloc( TRUE );

	//----------------------------------------------------------------------

	// create workbench object
	wm_object_workbench = wm_object_create( 0, 0, framebuffer.width_pixel, framebuffer.height_pixel );

	// fill workbench with default gradient
	uint32_t *pixel = (uint32_t *) ((uintptr_t) wm_object_workbench -> descriptor + sizeof( struct WM_STRUCTURE_DESCRIPTOR ));
	for( uint16_t y = 0; y < wm_object_workbench -> height; y++ )
		for( uint16_t x = 0; x < wm_object_workbench -> width; x++ )
			pixel[ (y * wm_object_workbench -> width) + x ] = STD_COLOR_BLACK;

	// object created
	wm_object_workbench -> descriptor -> flags |= WM_OBJECT_FLAG_visible | WM_OBJECT_FLAG_fixed_xy | WM_OBJECT_FLAG_fixed_z | WM_OBJECT_FLAG_flush;
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize environment
	wm_init();

	// hold the door
	while( TRUE ) {
		// check which objects have been recently updated
		wm_object();

		// connect zones with objects
		wm_zone();

		// fill zones with fragments of objects
		wm_fill();

		// synchronize workbench with framebuffer
		wm_sync();
	}
}
