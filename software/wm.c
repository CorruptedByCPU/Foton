/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
	#define	WM_OBJECT_limit		(STD_PAGE_byte / sizeof( struct WM_STRUCTURE_OBJECT ))
	#define	WM_OBJECT_NAME_length	256

	struct	WM_STRUCTURE_OBJECT {
		int16_t		x;
		int16_t		y;
		uint16_t	width_pixel;
		uint16_t	height_pixel;
		uint32_t	*base_address;
		uint64_t	size_byte;
		uint16_t	flags;
		uint8_t		id;
		uint8_t		length;
		int64_t		pid;
		uint8_t		name[ WM_OBJECT_NAME_length ];
	};

	struct	WM_STRUCTURE_ZONE {
		int16_t		x;
		int16_t		y;
		uint16_t	width;
		uint16_t	height;
		uintptr_t	object_address;
	};

	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	int64_t	wm_pid = EMPTY;

	struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER framebuffer;

	struct WM_STRUCTURE_OBJECT *wm_object_base_address = EMPTY;
	struct WM_STRUCTURE_ZONE *wm_zone_base_address = EMPTY;
	uintptr_t *wm_list_base_address = EMPTY;

	uint64_t wm_object_count = EMPTY;

struct WM_STRUCTURE_OBJECT *wm_object_create( int16_t x, int16_t y, uint16_t width, uint16_t height ) {
	// search for empty record in table
	for( uint64_t i = 0; i < wm_object_limit; i++ ) {
		// free of use?
		if( ! wm_object_base_address[ i ].flags ) {
			// set flags for window
			wm_object_base_address[ i ].flags = flags | WM_OBJECT_FLAG_exist;

			// fill record with window properties
			wm_object_base_address[ i ].x = x;
			wm_object_base_address[ i ].y = y;
			wm_object_base_address[ i ].width = width;
			wm_object_base_address[ i ].height = height;
			wm_object_base_address[ i ].size_byte = (width * height) << STATIC_VIDEO_DEPTH_shift;
			wm_object_base_address[ i ].id = ++wm_object_id;
			wm_object_base_address[ i ].pid = EMPTY;

			// set default object name
			wm_object_base_address[ i ].length = 0;
			for( uint8_t n = 0; n < sizeof( wm_object_name_default ) - 1; n++ ) wm_object_base_address[ i ].name[ n ] = wm_object_name_default[ n ];

			// register window on object list
			wm_object_insert( (struct WM_STRUCTURE_OBJECT *) &wm_object_base_address[ i ] );

			// newly created object becomes active
			wm_object_active = (struct WM_STRUCTURE_OBJECT *) &wm_object_base_address[ i ];

			// ready
			return (uint64_t) &wm_object_base_address[ i ];
		}
	}

	// error
	return EMPTY;
}

void wm_init( void ) {
	// get our PID number
	wm_pid = std_syscall_pid();

	// obtain information about kernel framebuffer
	std_syscall_framebuffer( &framebuffer );

	// framebuffer locked?
	if( framebuffer.owner_pid != wm_pid ) { log( "WM: Framebuffer is already in use. Exit." ); while( TRUE ); }

	// prepare space for an array of objects
	wm_object_base_address = (struct WM_STRUCTURE_OBJECT *) malloc( TRUE );

	// prepare space for an array of object list
	wm_list_base_address = (uintptr_t *) malloc( TRUE );

	// prepare space for an list of zones
	wm_zone_base_address = (struct WM_STRUCTURE_ZONE *) malloc( TRUE );

	// create workbench object
	wm_object_workbench = (struct WM_STRUCTURE_OBJECT *) wm_object_create( 0, 0, wm_object_framebuffer.width, wm_object_framebuffer.height, WM_OBJECT_FLAG_visible | WM_OBJECT_FLAG_fixed_z | WM_OBJECT_FLAG_fixed_xy );
	wm_object_workbench -> base_address = (uint32_t *) lib_sys_memory_alloc( wm_object_framebuffer.size_byte + sizeof( struct WM_STRUCTURE_DESCRIPTOR ) ) + (sizeof( struct WM_STRUCTURE_DESCRIPTOR ) >> STATIC_DIVIDE_BY_DWORD_shift);

}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize environment
	wm_init();

	// hold the door
	while( TRUE );
}
