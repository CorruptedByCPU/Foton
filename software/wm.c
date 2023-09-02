/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// constants, structures, definitions
	//----------------------------------------------------------------------
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

	struct WM_STRUCTURE_OBJECT	*wm_object_base_address	= EMPTY;
	struct WM_STRUCTURE_ZONE	*wm_zone_base_address	= EMPTY;
	uintptr_t 			*wm_list_base_address	= EMPTY;

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
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// initialize environment
	wm_init();

	// hold the door
	while( TRUE );
}
