/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_event( void ) {
	// incomming/outgoing messages
	uint8_t ipc_data[ STD_IPC_SIZE_byte ];

	// check every incomming message
	uint64_t source_pid = EMPTY;
	while( (source_pid = std_ipc_receive_by_type( (uint8_t *) &ipc_data, STD_IPC_TYPE_default )) ) {
		// properties of request
		struct STD_STRUCTURE_IPC_WINDOW *request = (struct STD_STRUCTURE_IPC_WINDOW *) &ipc_data;

		// choose behavior
		switch( request -> properties ) {
			case STD_IPC_WINDOW_create: {
				// properties of window create request
				struct STD_STRUCTURE_IPC_WINDOW_CREATE *create = (struct STD_STRUCTURE_IPC_WINDOW_CREATE *) &ipc_data;

				// properties of answer
				struct STD_STRUCTURE_IPC_WINDOW_DESCRIPTOR *answer = (struct STD_STRUCTURE_IPC_WINDOW_DESCRIPTOR *) &ipc_data;

				// properties of new object
				struct WM_STRUCTURE_OBJECT *object = EMPTY;

				// pointer to shared object descriptor
				uintptr_t descriptor = EMPTY;

				// if request is valid
				if( create -> width && create -> height ) {
					// try to create new object
					object = wm_object_create( create -> x, create -> y, create -> width, create -> height );

					// if created properly
					if( object )
						// try to share object descriptor with process
						descriptor = std_memory_share( source_pid, (uintptr_t) object -> descriptor, MACRO_PAGE_ALIGN_UP( object -> size_byte ) >> STD_SHIFT_PAGE, TRUE );
				}

				// if everything was done properly
				if( object && descriptor ) {
					// update PID of object
					object -> pid = source_pid;

					// and return object descriptor
					answer -> descriptor = descriptor;
				} else
					// reject window creation
					answer -> descriptor = EMPTY;

				// send answer
				std_ipc_send( source_pid, (uint8_t *) answer );

				// done
				break;
			}

			case STD_IPC_WINDOW_list: {
				// properties of window list request
				struct STD_STRUCTURE_IPC_WINDOW_CREATE *list = (struct STD_STRUCTURE_IPC_WINDOW_CREATE *) &ipc_data;

				// properties of answer
				struct STD_STRUCTURE_IPC_WINDOW_DESCRIPTOR *answer = (struct STD_STRUCTURE_IPC_WINDOW_DESCRIPTOR *) &ipc_data;

				// prepare object list
				struct LIB_WINDOW_STRUCTURE_LIST *entry = (struct LIB_WINDOW_STRUCTURE_LIST *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( (wm_list_limit + TRUE) * sizeof( struct LIB_WINDOW_STRUCTURE_LIST ) ) >> STD_SHIFT_PAGE );	// last list element is empty
				if( ! entry )
					// return empty list
					answer -> descriptor = EMPTY;
				else {
					// fill up will all existing objects
					for( uint64_t i = TRUE; i < wm_list_limit; i++ ) {
						// ignore entries of source
						if( wm_list_base_address[ i ] -> pid == source_pid ) continue;

						// object id
						entry[ i ].id = (uintptr_t) wm_list_base_address[ i ] - (uintptr_t) wm_object_base_address;

						// its flags
						entry[ i ].flags = wm_list_base_address[ i ] -> descriptor -> flags;

						// and name
						for( uint8_t n = INIT; n < wm_list_base_address[ i ] -> descriptor -> name_length; ) entry[ i ].name[ entry[ i ].name_limit++ ] = wm_list_base_address[ i ] -> descriptor -> name[ n ]; entry[ i ].name[ entry -> name_limit ] = STD_ASCII_TERMINATOR;
					}

					// move list area to process environment
					answer -> descriptor = std_memory_move( source_pid, (uintptr_t) entry, MACRO_PAGE_ALIGN_UP( (wm_list_limit + TRUE) * sizeof( struct LIB_WINDOW_STRUCTURE_LIST ) ) >> STD_SHIFT_PAGE );
				}

				// send answer
				std_ipc_send( source_pid, (uint8_t *) answer );

				// done
				break;
			}
		}
	}

	// check keyboard cache
	uint16_t key = std_keyboard();

	// incomming key?
	if( key ) {
		// properties of keyboard message
		struct STD_STRUCTURE_IPC_KEYBOARD *keyboard = (struct STD_STRUCTURE_IPC_KEYBOARD *) &ipc_data;

		// IPC type
		keyboard -> ipc.type = STD_IPC_TYPE_keyboard;
		keyboard -> key = key;	// and key code

		// action taken state
		uint8_t send = TRUE;

		// remember state of special behavior - key, or take action immediately
		switch( key ) {
			// left ctrl pressed
			case STD_KEY_CTRL_LEFT: { wm_keyboard_ctrl_left = TRUE; break; }

			// left ctrl released
			case STD_KEY_CTRL_LEFT | 0x80: { wm_keyboard_ctrl_left = FALSE; break; }

			// // tab pressed
			// case STD_KEY_TAB: {
			// 	// if left ctrl key is on hold
			// 	if( ! wm_keyboard_ctrl_left ) break;	// nope

			// 	// search forward for object to show
			// 	for( uint16_t i = INIT; i < wm_list_limit; i++ ) { wm_object_selected = wm_list_base_address[ i ]; break; }

			// 	// move it up
			// 	if( wm_object_move_up( wm_object_selected ) ) {
			// 		// force object to be visible
			// 		wm_object_selected -> descriptor -> flags |= STD_WINDOW_FLAG_visible;

			// 		// redraw object
			// 		wm_object_selected -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

			// 		// cursor pointer may be obscured, redraw
			// 		if( wm_object_cursor ) wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

			// 		// set as active
			// 		wm_object_active = wm_object_selected;
			// 	}

			// 	// ignore key
			// 	send = FALSE;

			// 	// done
			// 	break;
			// }
		}

		// send event to active object process
		if( send ) std_ipc_send( wm_object_active -> pid, (uint8_t *) keyboard );
	}

	//----------------------------------------------------------------------

	// retrieve current mouse status and position
	struct STD_STRUCTURE_MOUSE_SYSCALL mouse;
	std_mouse( (struct STD_STRUCTURE_MOUSE_SYSCALL *) &mouse );

	// calculate delta of cursor new position
	int16_t	delta_x = EMPTY;
	int16_t	delta_y = EMPTY;
	int8_t	delta_z = EMPTY;

	// select object under cursor position
	struct WM_STRUCTURE_OBJECT *object = wm_object_find( mouse.x, mouse.y, FALSE );

	// if cursor object exist
	if( wm_object_cursor ) { 
		// calculate delta of cursor new position
		delta_x = mouse.x - wm_object_cursor -> x;
		delta_y = mouse.y - wm_object_cursor -> y;
		delta_z = mouse.z - wm_mouse_z; wm_mouse_z = mouse.z;

		//--------------------------------------------------------------

		// update pointer position
		object -> descriptor -> x = (wm_object_cursor -> x + delta_x) - object -> x;
		object -> descriptor -> y = (wm_object_cursor -> y + delta_y) - object -> y;
		object -> descriptor -> z = delta_z;
	}

	//----------------------------------------------------------------------

	// left mouse button pressed?
	if( mouse.status & STD_MOUSE_BUTTON_left ) {
		// isn't holded down?
		if( ! wm_mouse_button_left ) {
			// remember mouse button state
			wm_mouse_button_left = TRUE;

			// select object under cursor position
			wm_object_selected = object;

			// if cursor over selected object is in place of possible header
			if( wm_object_selected -> descriptor -> y < wm_object_selected -> descriptor -> header_height ) wm_object_drag_allow = TRUE;

			// check if object can be moved along Z axis
			if( ! (wm_object_selected -> descriptor -> flags & STD_WINDOW_FLAG_fixed_z) && ! wm_keyboard_ctrl_left ) {
				// move object up inside list
				if( wm_object_move_up( wm_object_selected ) ) {
					// redraw object
					wm_object_selected -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

					// cursor pointer may be obscured, redraw
					if( wm_object_cursor ) wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
				}
			}

			// make object as active if not with taskbar flag
			if( ! (wm_object_selected -> descriptor -> flags & STD_WINDOW_FLAG_taskbar) ) wm_object_active = wm_object_selected;

			//------------------------------------------------------

			// properties of mouse message
			struct STD_STRUCTURE_IPC_MOUSE *ipc_mouse = (struct STD_STRUCTURE_IPC_MOUSE *) &ipc_data;

			// default values
			ipc_mouse -> ipc.type = STD_IPC_TYPE_mouse;
			ipc_mouse -> scroll = EMPTY;

			// left mouse button pressed
			ipc_mouse -> button = STD_IPC_MOUSE_BUTTON_left;

			// send event to selected object process
			std_ipc_send( wm_object_selected -> pid, (uint8_t *) ipc_mouse );

			//------------------------------------------------------

			// hide all unstable objects
			for( uint64_t i = INIT; i < wm_list_limit; i++ ) {
				// unstable object?
				if( ! (wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_unstable) ) continue;	// no
				
				// don't show anymore
				wm_list_base_address[ i ] -> descriptor -> flags &= ~STD_WINDOW_FLAG_visible;

				// and redraw area behind
				wm_list_base_address[ i ] -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
			}
		}
	} else {
		// left mouse button was on hold, but no left CTRL key?
		if( wm_mouse_button_left && ! wm_keyboard_ctrl_left ) {
			// properties of mouse message
			struct STD_STRUCTURE_IPC_MOUSE *ipc_mouse = (struct STD_STRUCTURE_IPC_MOUSE *) &ipc_data;
	
			// default values
			ipc_mouse -> ipc.type = STD_IPC_TYPE_mouse;
			ipc_mouse -> scroll = EMPTY;
	
			// left mouse button released
			ipc_mouse -> button = ~STD_IPC_MOUSE_BUTTON_left;
	
			// send event to selected object process
			std_ipc_send( wm_object_selected -> pid, (uint8_t *) ipc_mouse );
		}

		// release mouse button state
		wm_mouse_button_left = FALSE;

		// by default object is not draggable
		wm_object_drag_allow = FALSE;
	}

	//----------------------------------------------------------------------

	// right mouse button pressed?
	if( mouse.status & STD_IPC_MOUSE_BUTTON_right ) {
		// current object resizable?
		if( object -> descriptor -> flags & STD_WINDOW_FLAG_resizable ) {	// yes
			// left ALT key is hold and first occurence
			if( wm_keyboard_ctrl_left && ! wm_object_resize_init ) {	// yes
				// start resize procedure
				wm_object_resize_init = TRUE;

				// set current object
				tiwyn -> resized = object;

				// select zone modification

				// by default, user holds right or bottom side
				tiwyn -> direction.width = TRUE;
				tiwyn -> direction.height = TRUE;

				// is it left side?
				if( tiwyn -> resized -> descriptor -> x < (tiwyn -> resized -> width >> STD_SHIFT_2) ) tiwyn -> direction.x = TRUE;	// yes
				else tiwyn -> direction.x = FALSE;	// no

				// Y axis
				if( tiwyn -> resized -> descriptor -> y < (tiwyn -> resized -> height >> STD_SHIFT_2) ) tiwyn -> direction.y = TRUE;	// yes
				else tiwyn -> direction.y = FALSE;	// no

				// initialize hover object

				// create initial resize object
				wm_object_hover = wm_object_create( tiwyn -> resized -> x, tiwyn -> resized -> y, tiwyn -> resized -> width, tiwyn -> resized -> height );

				// mark it as our
				wm_object_hover -> pid = wm_pid;


				// show object
				wm_object_hover -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;
			}
		}
	} else {
		// stop hover phase
		wm_object_resize_init = FALSE;

		// if enabled
		if( wm_object_hover && wm_object_hover -> descriptor ) {
			// copy hover object properties to selected object
			tiwyn -> resized -> descriptor -> new_x		= wm_object_hover -> x;
			tiwyn -> resized -> descriptor -> new_y		= wm_object_hover -> y;
			tiwyn -> resized -> descriptor -> new_width	= wm_object_hover -> width;
			tiwyn -> resized -> descriptor -> new_height	= wm_object_hover -> height;

			// inform application interface about requested properties
			tiwyn -> resized -> descriptor -> flags |= STD_WINDOW_FLAG_properties;

			// remove hover object
			wm_object_hover -> descriptor -> flags = STD_WINDOW_FLAG_release;

			// relese pointer
			wm_object_hover = EMPTY;
		}
	}

	// // scroll movement?
	// if( delta_z ) {
	// 	// properties of mouse message
	// 	struct STD_STRUCTURE_IPC_MOUSE *ipc_mouse = (struct STD_STRUCTURE_IPC_MOUSE *) &ipc_data;

	// 	// default values
	// 	ipc_mouse -> ipc.type = STD_IPC_TYPE_mouse;
	// 	ipc_mouse -> scroll = delta_z;

	// 	// send event to selected object process
	// 	std_ipc_send( object -> pid, (uint8_t *) ipc_mouse );
	// }

	//--------------------------------------------------------------------------
	// if cursor pointer movement occurs
	if( delta_x || delta_y ) {
		// remove current cursor position from workbench
		wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm_object_cursor, FALSE );

		// update cursor position
		wm_object_cursor -> x = mouse.x;
		wm_object_cursor -> y = mouse.y;

		// redisplay cursor at new location
		wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

		// with left mouse button
		// left ALT key is on hold or not required?
		if( wm_mouse_button_left && (wm_keyboard_ctrl_left || wm_object_drag_allow) )
			// move object along with cursor pointer
			wm_object_move( delta_x, delta_y );

		// if hover object is created
		if( wm_object_resize_init && wm_object_hover ) {
			// remove old instance from cache
			wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm_object_hover, FALSE );

			// release old object area
			std_memory_release( (uintptr_t) wm_object_hover -> descriptor, MACRO_PAGE_ALIGN_UP( wm_object_hover -> size_byte ) >> STD_SHIFT_PAGE );

			// left zone?
			if( tiwyn -> direction.x && tiwyn -> direction.width ) {
				// do not move hover zone
				if( wm_object_hover -> x + delta_x < wm_object_hover -> x + wm_object_hover -> width ) {
					wm_object_hover -> x += delta_x;
					wm_object_hover -> width -= delta_x;
				}
			} else wm_object_hover -> width += delta_x;	// right

			// up zone?
			if( tiwyn -> direction.y && tiwyn -> direction.height ) {
				// do not move hover zone
				if( wm_object_hover -> y + delta_y < wm_object_hover -> y + wm_object_hover -> height ) {
					wm_object_hover -> y += delta_y;
					wm_object_hover -> height -= delta_y;
				}
			} else wm_object_hover -> height += delta_y;	// down

			// do not allow object width/height less than 1 pixel
			if( wm_object_hover -> width < TRUE ) wm_object_hover -> width = TRUE;
			if( wm_object_hover -> height < TRUE ) wm_object_hover -> height = TRUE;

			// and larger than current screen properties
			if( wm_object_hover -> width > wm_object_cache.width ) wm_object_hover -> width = wm_object_cache.width;
			if( wm_object_hover -> height > wm_object_cache.height ) wm_object_hover -> height = wm_object_cache.height;

			// calculate new object area size in Bytes
			wm_object_hover -> size_byte = ( wm_object_hover -> width * wm_object_hover -> height * STD_VIDEO_DEPTH_byte) + sizeof( struct LIB_WINDOW_DESCRIPTOR );

			// assign new area for object
			wm_object_hover -> descriptor = (struct LIB_WINDOW_DESCRIPTOR *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( wm_object_hover -> size_byte ) >> STD_SHIFT_PAGE );

			// fill object with default pattern/color
			uint32_t *hover_pixel = (uint32_t *) ((uintptr_t) wm_object_hover -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));
			for( uint16_t y = 0; y < wm_object_hover -> height; y++ )
				for( uint16_t x = 0; x < wm_object_hover -> width; x++ )
					hover_pixel[ (y * wm_object_hover -> width) + x ] = 0x80008000;

			// and point border
			for( uint16_t y = 0; y < wm_object_hover -> height; y++ )
				for( uint16_t x = 0; x < wm_object_hover -> width; x++ )
					if( ! x || ! y || x == wm_object_hover -> width - 1 || y == wm_object_hover -> height - 1 ) hover_pixel[ (y * wm_object_hover -> width) + x ] = 0x80008000;

			// show object
			wm_object_hover -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;
		}
	}
}
