/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

int64_t wm_taskbar( void ) {
	// fill taskbar with default background color
	uint32_t *taskbar_pixel = (uint32_t *) ((uintptr_t) wm_object_taskbar -> descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ));
	for( uint16_t y = 0; y < wm_object_taskbar -> height; y++ )
		for( uint16_t x = 0; x < wm_object_taskbar -> width; x++ )
			taskbar_pixel[ (y * wm_object_taskbar -> width) + x ] = 0xD0000000;

	// object content ready for display
	wm_object_taskbar -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	// hold the door
	while( TRUE );
}