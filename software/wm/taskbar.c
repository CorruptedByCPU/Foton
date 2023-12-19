/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

// uint8_t wm_taskbar_insert( struct WM_STRUCTURE_OBJECT *object ) {
// 	// block access to taskbar list
// 	MACRO_LOCK( wm_taskbar_semaphore );

// 	// object already on taskbar list?
// 	for( uint64_t i = 0; i < wm_taskbar_limit; i++ )
// 		if( wm_taskbar_base_address[ i ] == object ) {
// 			// release access to taskbar list
// 			MACRO_UNLOCK( wm_taskbar_semaphore );

// 			// nothing to do
// 			return TRUE;
// 		}

// 	// try to extend objects list
// 	struct WM_STRUCTURE_OBJECT **wm_taskbar_base_address_tmp = (struct WM_STRUCTURE_OBJECT **) realloc( wm_taskbar_base_address, sizeof( struct WM_STRUCTURE_OBJECT * ) * (wm_taskbar_limit + 1) );
// 	if( ! wm_taskbar_base_address_tmp ) { MACRO_UNLOCK( wm_taskbar_semaphore ); return FALSE; }	// cannot do that

// 	// update new object list pointer
// 	wm_taskbar_base_address = wm_taskbar_base_address_tmp;

// 	// taskbar list extended, insert object pointer
// 	wm_taskbar_base_address[ wm_taskbar_limit++ ] = object;

// 	// release access to object list
// 	MACRO_UNLOCK( wm_taskbar_semaphore );

// 	// ready
// 	return TRUE;
// }

// void wm_taskbar_list_entry( struct WM_STRUCTURE_OBJECT *object, uint16_t x, uint16_t width ) {
// 	// debug
// 	if( ! object -> descriptor ) return;

// 	// properties of entry content area
// 	uint32_t *entry_pixel = (uint32_t *) ((uintptr_t) wm_object_taskbar -> descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR )) + x;

// 	// select default background color for entry
// 	uint32_t color = WM_TASKBAR_BG_invisible;

// 	// or
// 	if( object -> descriptor -> flags & STD_WINDOW_FLAG_visible ) color = WM_TASKBAR_BG_visible;

// 	// fill element with default background color
// 	for( uint16_t row = 0; row < wm_object_taskbar -> height; row++ )
// 		for( uint16_t col = 0; col < width - 1; col++ )
// 			entry_pixel[ (row * wm_object_taskbar -> width) + col ] = color;

// 	// mark active window
// 	if( object == wm_object_active )
// 		for( uint16_t row = wm_object_taskbar -> height - 2; row < wm_object_taskbar -> height; row++ )
// 			for( uint16_t col = 0; col < width - 1; col++ )
// 				entry_pixel[ (row * wm_object_taskbar -> width) + col ] = lib_color( 40 );

// 	// set entry name
// 	lib_font( LIB_FONT_FAMILY_ROBOTO, object -> descriptor -> name, object -> descriptor -> length, 0xFFFFFFFF, entry_pixel + (4 * wm_object_taskbar -> width) + 4, wm_object_taskbar -> width, LIB_FONT_ALIGN_left );
// }

void wm_taskbar_list( void ) {
	// fill taskbar with default background color
	uint32_t *taskbar_pixel = (uint32_t *) ((uintptr_t) wm_object_taskbar -> descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ));
	for( uint16_t y = 0; y < wm_object_taskbar -> height; y++ )
		for( uint16_t x = 0; x < wm_object_taskbar -> width; x++ )
			taskbar_pixel[ (y * wm_object_taskbar -> width) + x ] = WM_TASKBAR_BG_default;

	// show menu buton on taskbar
	uint8_t test[ 3 ] = "|||";
	lib_font( LIB_FONT_FAMILY_ROBOTO, (uint8_t *) &test, sizeof( test ), 0xFFFFFFFF, taskbar_pixel + (((WM_OBJECT_TASKBAR_HEIGHT_pixel - LIB_FONT_HEIGHT_pixel) / 2) * wm_object_taskbar -> width) + (22 >> STD_SHIFT_2), wm_object_taskbar -> width, LIB_FONT_ALIGN_center );

	// block access to object list
	// MACRO_LOCK( wm_list_semaphore );

	// // count current amount of objects to show up
	// for( uint64_t i = 0; i < wm_list_limit; i++ ) {
	// 	// its own object?
	// 	if( wm_list_base_address[ i ] -> pid == wm_pid ) continue;	// omit

	// 	// add object to taskbar list
	// 	wm_taskbar_insert( (struct WM_STRUCTURE_OBJECT *) wm_list_base_address[ i ] );
	// }

	// // there are objects?
	// if( wm_taskbar_limit ) {
	// 	// calculate width of entry
	// 	wm_taskbar_entry_width = (wm_object_taskbar -> width - WM_OBJECT_TASKBAR_HEIGHT_pixel) / wm_taskbar_limit;

	// 	// if entry wider than allowed
	// 	if( wm_taskbar_entry_width > WM_OBJECT_TASKBAR_ENTRY_pixel ) wm_taskbar_entry_width = WM_OBJECT_TASKBAR_ENTRY_pixel;	// limit it

	// 	// first entry position
	// 	uint16_t x = WM_OBJECT_TASKBAR_HEIGHT_pixel;

	// 	// for every entry
	// 	for( uint8_t i = 0; i < wm_taskbar_limit; i++ ) {
	// 		// last entry width, align
	// 		// if( i + 1 == wm_taskbar_limit ) wm_taskbar_list_entry( wm_taskbar_base_address[ i ], x, wm_taskbar_entry_width + (wm_object_taskbar -> width - WM_OBJECT_TASKBAR_HEIGHT_pixel) % wm_taskbar_limit );

	// 		// show on taskbar list
	// 		wm_taskbar_list_entry( wm_taskbar_base_address[ i ], x, wm_taskbar_entry_width );

	// 		// next entry position
	// 		x += wm_taskbar_entry_width;
	// 	}
	// }

	// release access to object list
	// MACRO_UNLOCK( wm_list_semaphore );

	// update taskbar content on screen
	wm_object_taskbar -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
}

void wm_taskbar( void ) {
	// if there was significant modification on object list/array
	if( ! wm_taskbar_modified ) return;	// no

	// taskbar object content update in progress
	wm_taskbar_modified = FALSE;

	// update taskbar list content/status
	wm_taskbar_list();
}



		// // // retrieve incomming message
		// // uint8_t data[ STD_IPC_SIZE_byte ];
		// // if( std_ipc_receive( (uint8_t *) &data ) ) {
		// // 	// properties of message
		// // 	struct STD_IPC_STRUCTURE_DEFAULT *ipc = (struct STD_IPC_STRUCTURE_DEFAULT *) &data;

		// // 	// message type: mouse?
		// // 	if( ipc -> type == STD_IPC_TYPE_mouse ) {	// yes
		// // 		// properties of mouse message
		// // 		struct STD_IPC_STRUCTURE_MOUSE *mouse = (struct STD_IPC_STRUCTURE_MOUSE *) &data;

		// // 		// button: left mouse pressed?
		// // 		if( mouse -> button & STD_IPC_MOUSE_BUTTON_left ) {	// yes
		// // 			// there are entries on taskbar list?
		// // 			if( wm_taskbar_limit ) {
		// // 				// mouse pointer in range of any taskbar list entry?
		// // 				if( wm_object_taskbar -> descriptor -> x >= WM_OBJECT_TASKBAR_HEIGHT_pixel && (wm_object_taskbar -> descriptor -> x - WM_OBJECT_TASKBAR_HEIGHT_pixel) / wm_taskbar_entry_width < wm_taskbar_limit ) {
		// // 					// properties of selected object entry
		// // 					struct WM_STRUCTURE_OBJECT *object = wm_taskbar_base_address[ (wm_object_taskbar -> descriptor -> x - WM_OBJECT_TASKBAR_HEIGHT_pixel) / wm_taskbar_entry_width ];

		// // 					// its an active object?
		// // 					if( object == wm_object_active ) {
		// // 						// hide it
		// // 						object -> descriptor -> flags |= STD_WINDOW_FLAG_minimize;

		// // 						// mark object as invisible
		// // 						object -> descriptor -> flags &= ~STD_WINDOW_FLAG_visible;

		// // 						// and select new active object
		// // 						wm_object_active_new();
		// // 					} else {
		// // 						// move object up inside list
		// // 						wm_object_move_up( object );

		// // 						// redraw object on screen
		// // 						object -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

		// // 						// mark as new active
		// // 						wm_object_active = object;
		// // 					}

		// // 					// update taskbar list content
		// // 					wm_taskbar_modified = TRUE;
		// // 				}
		// // 			}
		// // 		}
		// // 	}
		// // }