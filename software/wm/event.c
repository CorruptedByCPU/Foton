/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_event( void ) {
	// incomming message
	uint8_t data[ STD_IPC_SIZE_byte ]; int64_t source = EMPTY;
	while( (source = std_ipc_receive( (uint8_t *) &data )) ) {
		// event to parse?
		if( data[ offsetof( struct STD_IPC_STRUCTURE_DEFAULT, type ) ] != STD_IPC_TYPE_event ) break;	// no

		// properties of request
		struct STD_IPC_STRUCTURE_WINDOW *request = (struct STD_IPC_STRUCTURE_WINDOW *) &data;

		// properties of answer
		struct STD_IPC_STRUCTURE_WINDOW_DESCRIPTOR *answer = (struct STD_IPC_STRUCTURE_WINDOW_DESCRIPTOR *) &data;

		// if request is invalid
		if( ! request -> width || ! request -> height ) {
			// reject window creation
			answer -> descriptor = EMPTY;

			// send answer
			std_ipc_send( source, (uint8_t *) answer );

			// next request
			continue;
		}

		// create new object for process
		struct WM_STRUCTURE_OBJECT *object = wm_object_create( request -> x, request -> y, request -> width, request -> height );

		// update ID of process owning object
		object -> pid = source;

		// share new object descriptor with process
		uintptr_t descriptor = EMPTY;
		if( ! (descriptor = std_memory_share( source, (uintptr_t) object -> descriptor, MACRO_PAGE_ALIGN_UP( object -> size_byte ) >> STD_SHIFT_PAGE )) ) continue;	// no enough memory?

		// send answer
		answer -> descriptor = descriptor;
		std_ipc_send( source, (uint8_t *) answer );
	}

	// check keyboard cache
	uint16_t key = std_keyboard();

	// incomming key?
	if( key ) {
		// properties of keyboard message
		struct STD_IPC_STRUCTURE_KEYBOARD *message = (struct STD_IPC_STRUCTURE_KEYBOARD *) &data;

		// IPC type
		message -> ipc.type = STD_IPC_TYPE_keyboard;
		message -> key = key;	// and key code

		// remember state of special behavior - key, or take action immediately
		switch( key ) {
			// left alt pressed
			case STD_KEY_ALT_LEFT: { wm_keyboard_status_alt_left = TRUE; break; }

			// left alt released
			case STD_KEY_ALT_LEFT | 0x80: { wm_keyboard_status_alt_left = FALSE; break; }

			// shift left pressed
			case STD_KEY_SHIFT_LEFT: { wm_keyboard_status_shift_left = TRUE; break; }
		
			// shift left released
			case STD_KEY_SHIFT_LEFT | 0x80: { wm_keyboard_status_shift_left = FALSE; break; }

			// tab pressed
			case STD_KEY_TAB: {
				// if left alt key is holded
				if( ! wm_keyboard_status_alt_left ) break;	// nope

				// unselect any object before shifting
				wm_object_selected = EMPTY;

				// search forward for object to show
				for( uint16_t i = 0; i < wm_list_limit; i++ ) if( wm_list_base_address[ i ] -> pid != wm_pid ) { wm_object_selected = wm_list_base_address[ i ]; break; }

				// move it up
				if( wm_object_move_up() ) {
					// force object to be visible
					wm_object_selected -> descriptor -> flags |= STD_WINDOW_FLAG_visible;

					// redraw object
					wm_object_selected -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

					// cursor pointer may be obscured, redraw
					wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
				}

				// set as active, if found
				if( wm_object_selected ) wm_object_active = wm_object_selected;

				// done
				break;
			}
		}
		

		// send key to active object process
		std_ipc_send( wm_object_active -> pid, (uint8_t *) message );
	}

	// if cursor object doesn't exist
	if( ! wm_object_cursor ) return;	// nothing to do

	// retrieve current mouse status and position
	struct STD_SYSCALL_STRUCTURE_MOUSE mouse;
	std_mouse( (struct STD_SYSCALL_STRUCTURE_MOUSE *) &mouse );

	// calculate delta of cursor new position
	int16_t delta_x = mouse.x - wm_object_cursor -> x;
	int16_t delta_y = mouse.y - wm_object_cursor -> y;

	//--------------------------------------------------------------------------
	// left mouse button pressed?
	if( mouse.status & STD_MOUSE_BUTTON_left ) {
		// isn't holded down?
		if( ! wm_mouse_button_left_semaphore ) {
			// remember mouse button state
			wm_mouse_button_left_semaphore = TRUE;

			// select object under cursor position
			wm_object_selected = wm_object_find( mouse.x, mouse.y, FALSE );

			// check if object can be moved along Z axis
			if( ! (wm_object_selected -> descriptor -> flags & STD_WINDOW_FLAG_fixed_z) && ! wm_keyboard_status_alt_left ) {
				// move object up inside list
				if( wm_object_move_up() ) {
					// redraw object
					wm_object_selected -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

					// cursor pointer may be obscured, redraw
					wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
				}
			}

			// make object as active if left ALT key is not holded
			if( ! wm_keyboard_status_alt_left ) wm_object_active = wm_object_selected;
		}
	} else
		// release mouse button state
		wm_mouse_button_left_semaphore = FALSE;

	//--------------------------------------------------------------------------
	// if cursor pointer movement occurs
	if( delta_x || delta_y ) {
		// remove current cursor position from workbench
		wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm_object_cursor, FALSE );

		// // update cursor position
		wm_object_cursor -> x = mouse.x;
		wm_object_cursor -> y = mouse.y;

		// // redisplay the cursor at the new location
		wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

		// if object selected and left mouse button is held with left alt key
		if( wm_object_selected && wm_mouse_button_left_semaphore && wm_keyboard_status_alt_left )
			// move the object along with the cursor pointer
			wm_object_move( delta_x, delta_y );
	}
}