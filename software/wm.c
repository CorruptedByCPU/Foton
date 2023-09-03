/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	#define	WM_OBJECT_FLAG_flush	0b0000000000000001
	#define	WM_OBJECT_FLAG_visible	0b0000000000000010
	#define	WM_OBJECT_FLAG_fixed_xy	0b0000000000000100
	#define	WM_OBJECT_FLAG_fixed_z	0b0000000000001000

	struct	WM_STRUCTURE_OBJECT {
		int16_t		x_pixel;
		int16_t		y_pixel;
		uint16_t	width_pixel;
		uint16_t	height_pixel;
		int64_t		pid;
		uint64_t	size_byte;
		struct WM_STRUCTURE_DESCRIPTOR	*descriptor;
	};

	struct	WM_STRUCTURE_ZONE {
		int16_t		x_pixel;
		int16_t		y_pixel;
		uint16_t	width_pixel;
		uint16_t	height_pixel;
		struct WM_STRUCTURE_OBJECT *object_address;
	};

	// structure must be divisible by 0x10
	struct	WM_STRUCTURE_DESCRIPTOR {
		uint16_t	flags;
		uint16_t	width_pixel;
		uint16_t	height_pixel;
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

	uint8_t wm_object_semaphore = FALSE;
	uint8_t wm_list_semaphore = FALSE;

	struct WM_STRUCTURE_OBJECT *wm_object_workbench = EMPTY;
	struct WM_STRUCTURE_OBJECT *wm_object_active = EMPTY;

void wm_object( void ) {
	// search whole list for object flush
	for( uint16_t i = 0; i < wm_list_limit; i++ ) {
		// object visible and requested flush?
		if( wm_list_base_address[ i ] -> descriptor -> flags & (WM_OBJECT_FLAG_visible | WM_OBJECT_FLAG_flush) ) {
			// parse whole object area
			// wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm_list_base_address[ i ], FALSE );

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

struct WM_STRUCTURE_OBJECT *wm_object_create( int16_t x_pixel, int16_t y_pixel, uint16_t width_pixel, uint16_t height_pixel ) {
	// block modification of object table
	while( __sync_val_compare_and_swap( &wm_object_semaphore, UNLOCK, LOCK ) );

	// find available entry or extend object table
	while( TRUE ) {
		// search for empty entry inside table
		for( uint64_t i = 0; i < wm_object_limit; i++ ) {
			// entry in use?
			if( ! wm_object_base_address[ i ].descriptor ) {	// no
				// fill object properties
				wm_object_base_address[ i ].x_pixel = x_pixel;
				wm_object_base_address[ i ].y_pixel = y_pixel;
				wm_object_base_address[ i ].width_pixel = width_pixel;
				wm_object_base_address[ i ].height_pixel = height_pixel;

				// calculate object area size in Bytes
				wm_object_base_address[ i ].size_byte = (width_pixel * height_pixel * STD_VIDEO_DEPTH_byte) + sizeof( struct WM_STRUCTURE_DESCRIPTOR );

				// assign area for object
				wm_object_base_address[ i ].descriptor = (struct WM_STRUCTURE_DESCRIPTOR *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( wm_object_base_address[ i ].size_byte ) >> STD_SHIFT_PAGE );

				// fill descriptor with known values
				wm_object_base_address[ i ].descriptor -> width_pixel = width_pixel;
				wm_object_base_address[ i ].descriptor -> height_pixel = height_pixel;

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
	for( uint16_t y = 0; y < wm_object_workbench -> height_pixel; y++ )
		for( uint16_t x = 0; x < wm_object_workbench -> width_pixel; x++ )
			pixel[ (y * wm_object_workbench -> width_pixel) + x ] = STD_COLOR_RED_light;

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
	}
}
