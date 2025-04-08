/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t wm_taskbar_insert( struct WM_STRUCTURE_OBJECT *object ) {
	// block access to taskbar list
	MACRO_LOCK( wm_taskbar_semaphore );

	// object already on taskbar list?
	for( uint64_t i = 0; i < wm_taskbar_limit; i++ )
		if( wm_taskbar_base_address[ i ] == object ) {
			// release access to taskbar list
			MACRO_UNLOCK( wm_taskbar_semaphore );

			// nothing to do
			return TRUE;
		}

	// insert object pointer
	wm_taskbar_base_address[ wm_taskbar_limit++ ] = object;

	// release access to object list
	MACRO_UNLOCK( wm_taskbar_semaphore );

	// ready
	return TRUE;
}

void wm_taskbar_list_entry( struct WM_STRUCTURE_OBJECT *object, uint16_t x, uint16_t width ) {
	// debug
	if( ! object -> descriptor ) return;

	// properties of entry content area
	uint32_t *entry_pixel = (uint32_t *) ((uintptr_t) wm_object_taskbar -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR )) + x;

	// select default background color for entry
	uint32_t color = WM_TASKBAR_BG_invisible;

	// or
	if( object -> descriptor -> flags & STD_WINDOW_FLAG_visible ) color = WM_TASKBAR_BG_visible;
	if( object == wm_object_active ) color = WM_TASKBAR_BG_active;

	// fill element with default background color
	for( uint16_t row = 0; row < wm_object_taskbar -> height; row++ )
		for( uint16_t col = 0; col < width - 1; col++ )
			entry_pixel[ (row * wm_object_taskbar -> width) + col ] = color;

	// mark active window
	if( object == wm_object_active )
		for( uint16_t row = wm_object_taskbar -> height - 2; row < wm_object_taskbar -> height; row++ )
			for( uint16_t col = 0; col < width - 1; col++ )
				entry_pixel[ (row * wm_object_taskbar -> width) + col ] = 0xFF008000;

	// set entry name
	lib_font( LIB_FONT_FAMILY_ROBOTO, object -> descriptor -> name, object -> descriptor -> name_length, 0xFFFFFFFF, entry_pixel + (4 * wm_object_taskbar -> width) + 4, wm_object_taskbar -> width, LIB_FONT_ALIGN_left );
}

void wm_taskbar_list( void ) {
	// fill taskbar with default background color
	uint32_t *taskbar_pixel = (uint32_t *) ((uintptr_t) wm_object_taskbar -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ));
	for( uint16_t y = 0; y < wm_object_taskbar -> height; y++ )
		for( uint16_t x = LIB_INTERFACE_HEADER_HEIGHT_pixel; x < wm_object_taskbar -> width - WM_OBJECT_TASKBAR_CLOCK_pixel; x++ )
			taskbar_pixel[ (y * wm_object_taskbar -> width) + x ] = WM_TASKBAR_BG_default;

	// block access to object list
	MACRO_LOCK( wm_list_semaphore );

	// count current amount of objects to show up
	for( uint64_t i = 0; i < wm_list_limit; i++ ) {
		// its own object?
		if( wm_list_base_address[ i ] -> pid == wm_pid || wm_list_base_address[ i ] -> pid == wm_object_taskbar -> pid ) continue;	// omit

		// add object to taskbar list
		wm_taskbar_insert( (struct WM_STRUCTURE_OBJECT *) wm_list_base_address[ i ] );
	}

	// there are objects?
	if( wm_taskbar_limit ) {
		// calculate width of entry
		wm_taskbar_entry_width = (wm_object_taskbar -> width - (WM_OBJECT_TASKBAR_HEIGHT_pixel + WM_OBJECT_TASKBAR_CLOCK_pixel)) / wm_taskbar_limit;

		// if entry wider than allowed
		if( wm_taskbar_entry_width > WM_OBJECT_TASKBAR_ENTRY_pixel ) wm_taskbar_entry_width = WM_OBJECT_TASKBAR_ENTRY_pixel;	// limit it

		// first entry position
		uint16_t x = WM_OBJECT_TASKBAR_HEIGHT_pixel;

		// for every entry
		for( uint8_t i = 0; i < wm_taskbar_limit; i++ ) {
			// last entry width, align
			if( i + 1 == wm_taskbar_limit ) wm_taskbar_list_entry( wm_taskbar_base_address[ i ], x, wm_taskbar_entry_width + (wm_object_taskbar -> width - (WM_OBJECT_TASKBAR_HEIGHT_pixel + WM_OBJECT_TASKBAR_CLOCK_pixel)) % wm_taskbar_limit );
			else
				// show on taskbar list
				wm_taskbar_list_entry( wm_taskbar_base_address[ i ], x, wm_taskbar_entry_width );

			// next entry position
			x += wm_taskbar_entry_width;
		}
	}

	// release access to object list
	MACRO_UNLOCK( wm_list_semaphore );
}

int64_t wm_taskbar( void ) {
	// main loop
	while( TRUE ) {
		// free up AP time
		sleep( TRUE );

		// incomming message
		uint8_t ipc_data[ STD_IPC_SIZE_byte ];

		// receive pending messages
		if( std_ipc_receive_by_type( (uint8_t *) &ipc_data, STD_IPC_TYPE_mouse ) ) {
			// message properties
			struct STD_STRUCTURE_IPC_MOUSE *mouse = (struct STD_STRUCTURE_IPC_MOUSE *) &ipc_data;

			// released left mouse button?
			if( mouse -> button == STD_IPC_MOUSE_BUTTON_left ) {
				// there are entries on taskbar list?
				if( wm_taskbar_limit ) {
					// mouse pointer in range of any taskbar list entry?
					if( wm_object_taskbar -> descriptor -> x >= WM_OBJECT_TASKBAR_HEIGHT_pixel && (wm_object_taskbar -> descriptor -> x - WM_OBJECT_TASKBAR_HEIGHT_pixel) / wm_taskbar_entry_width < wm_taskbar_limit ) {
						// properties of selected object entry
						struct WM_STRUCTURE_OBJECT *object = wm_taskbar_base_address[ (wm_object_taskbar -> descriptor -> x - WM_OBJECT_TASKBAR_HEIGHT_pixel) / wm_taskbar_entry_width ];

						// its an active object?
						if( object == wm_object_active ) {
							// hide it
							object -> descriptor -> flags |= STD_WINDOW_FLAG_minimize;

							// mark object as invisible
							object -> descriptor -> flags &= ~STD_WINDOW_FLAG_visible;

							// and select new active object
							wm_object_active_new();
						} else {
							// move object up inside list
							wm_object_move_up( object );

							// redraw object on screen
							object -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

							// mark as new active
							wm_object_active = object;
						}

						// update taskbar list content
						wm_taskbar_modified = TRUE;
					}
				}
			}
		}

		// if there was significant modification on object list/array
		if( wm_taskbar_modified ) {
			// taskbar object content update in progress
			wm_taskbar_modified = FALSE;

			// update taskbar list content/status
			wm_taskbar_list();

			// update taskbar content on screen
			wm_object_taskbar -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
		}
	}
}
