/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void tiwyn_init( void ) {
	// Tiwyn environment global variables/functions/rountines
	tiwyn = (struct TIWYN_STRUCTURE *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct TIWYN_STRUCTURE ) ) >> STD_SHIFT_PAGE );

	// disable window management, for now
	tiwyn -> enable = FALSE;

	// retrieve our process ID
	tiwyn -> pid = std_pid();

	// retrieve properties of kernel framebuffer
	std_framebuffer( (struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER *) &tiwyn -> framebuffer );

	// assign area for an array of objects
	tiwyn -> object = (struct TIWYN_STRUCTURE_OBJECT *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct TIWYN_STRUCTURE_OBJECT ) * TIWYN_OBJECT_LIMIT ) >> STD_SHIFT_PAGE );

	// assign area for a list of objects
	tiwyn -> list = (struct TIWYN_STRUCTURE_OBJECT **) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct TIWYN_STRUCTURE_OBJECT * ) * (TIWYN_LIST_LIMIT + TRUE) ) >> STD_SHIFT_PAGE );	// blank entry at end list

	// assign area for a list of zones
	tiwyn -> zone = (struct TIWYN_STRUCTURE_ZONE *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct TIWYN_STRUCTURE_ZONE ) * TIWYN_ZONE_LIMIT ) >> STD_SHIFT_PAGE );

	//----------------------------------------------------------------------

	// create cache area
	tiwyn -> canvas.width		= tiwyn -> framebuffer.width_pixel;
	tiwyn -> canvas.height		= tiwyn -> framebuffer.height_pixel;
	tiwyn -> canvas.descriptor	= (struct LIB_WINDOW_DESCRIPTOR *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( (tiwyn -> canvas.width * tiwyn -> canvas.height * STD_VIDEO_DEPTH_byte) + sizeof( struct LIB_WINDOW_DESCRIPTOR ) ) >> STD_SHIFT_PAGE );

	// leave cache untouched, first object synchronization will fill it up

	//----------------------------------------------------------------------

	// execute Desktop Environment in separate thread
	uint8_t tiwyn_string[] = "tiwyn - desktop environment";
	std_thread( (uintptr_t) &tiwyn_desktop, (uint8_t *) &tiwyn_string, sizeof( tiwyn_string ) - TRUE );

	// wait for desktop
	while( ! tiwyn -> enable ) sleep( TRUE );
}
