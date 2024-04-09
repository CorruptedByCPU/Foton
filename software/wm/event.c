/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_event( void ) {
	// // incomming message
	uint8_t data[ STD_IPC_SIZE_byte ]; int64_t source = EMPTY;
	while( (source = std_ipc_receive( (uint8_t *) &data )) ) {
		// event to parse?
		if( data[ offsetof( struct STD_IPC_STRUCTURE_DEFAULT, type ) ] != STD_IPC_TYPE_event ) break;	// no

		// properties of request
		struct STD_IPC_STRUCTURE_WINDOW *request = (struct STD_IPC_STRUCTURE_WINDOW *) &data;

		// properties of answer
		struct STD_IPC_STRUCTURE_WINDOW_DESCRIPTOR *answer = (struct STD_IPC_STRUCTURE_WINDOW_DESCRIPTOR *) &data;

		// properties of new object
		struct WM_STRUCTURE_OBJECT *object = EMPTY;

		// pointer to shared object descriptor
		uintptr_t descriptor = EMPTY;

		// if request is valid
		if( request -> width && request -> height ) {
			// try to create new object
			object = wm_object_create( request -> x, request -> y, request -> width, request -> height );

			// if created properly
			if( object )
				// try to share object descriptor with process
				descriptor = std_memory_share( source, (uintptr_t) object -> descriptor, MACRO_PAGE_ALIGN_UP( object -> size_byte ) >> STD_SHIFT_PAGE );
		}

		// if everything was done properly
		if( object && descriptor ) {
			// update PID of object
			object -> pid = source;

			// and return object descriptor
			answer -> descriptor = descriptor;		
		} else
			// reject window creation
			answer -> descriptor = EMPTY;

		// send answer
		std_ipc_send( source, (uint8_t *) answer );
	}

	// check keyboard cache
	uint16_t key = std_keyboard();

	// incomming key?
	if( key ) {
		// properties of keyboard message
		struct STD_IPC_STRUCTURE_KEYBOARD *keyboard = (struct STD_IPC_STRUCTURE_KEYBOARD *) &data;

		// IPC type
		keyboard -> ipc.type = STD_IPC_TYPE_keyboard;
		keyboard -> key = key;	// and key code

		// action taken state
		uint8_t send = TRUE;

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

			// ctrl left pressed
			case STD_KEY_CTRL_LEFT: { wm_keyboard_status_ctrl_left = TRUE; break; }

			// ctrl left released
			case STD_KEY_CTRL_LEFT | 0x80: { wm_keyboard_status_ctrl_left = FALSE; break; }

			// tab pressed
			case STD_KEY_TAB: {
				// if left alt key is holded
				if( ! wm_keyboard_status_alt_left ) break;	// nope

				// search forward for object to show
				for( uint16_t i = 0; i < wm_list_limit; i++ ) if( wm_list_base_address[ i ] -> pid != wm_pid ) { wm_object_selected = wm_list_base_address[ i ]; break; }

				// move it up
				if( wm_object_move_up( wm_object_selected ) ) {
					// force object to be visible
					wm_object_selected -> descriptor -> flags |= STD_WINDOW_FLAG_visible;

					// redraw object
					wm_object_selected -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

					// cursor pointer may be obscured, redraw
					wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

					// set as active
					wm_object_active = wm_object_selected;

					// update taskbar status
					wm_taskbar_modified = TRUE;
				}

				// done
				break;
			}

			// backslash pressed
			case STD_ASCII_BACKSLASH: {
				// left control key is on hold
				if( wm_keyboard_status_ctrl_left ) {
					// execute console application
					std_exec( (uint8_t *) "console", 7, EMPTY );

					// do not send this key to current process
					send = FALSE;
				}

				// done
				break;
			}
		}

		// send event to active object process
		if( send ) std_ipc_send( wm_object_active -> pid, (uint8_t *) keyboard );
	}

	// retrieve current mouse status and position
	struct STD_SYSCALL_STRUCTURE_MOUSE mouse;
	std_mouse( (struct STD_SYSCALL_STRUCTURE_MOUSE *) &mouse );

	// calculate delta of cursor new position
	int16_t delta_x = mouse.x - wm_object_cursor -> x;
	int16_t delta_y = mouse.y - wm_object_cursor -> y;

	//--------------------------------------------------------------------------

	// block access to object list
	MACRO_LOCK( wm_list_semaphore );

	// update cursor position over object (active only)
	for( uint64_t i = 0; i < wm_list_limit; i++ ) {
		// ignore hidden objects
		if( ! (wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_visible) ) continue;

		// ignore cursor object
		if( wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_cursor ) continue;

		// update pointer position
		wm_list_base_address[ i ] -> descriptor -> x = (wm_object_cursor -> x + delta_x) - wm_list_base_address[ i ] -> x;
		wm_list_base_address[ i ] -> descriptor -> y = (wm_object_cursor -> y + delta_y) - wm_list_base_address[ i ] -> y;
	}

	// release access to object list
	MACRO_UNLOCK( wm_list_semaphore );

	//--------------------------------------------------------------------------

	// left mouse button pressed?
	if( mouse.status & STD_MOUSE_BUTTON_left ) {
		// isn't holded down?
		if( ! wm_mouse_button_left_semaphore ) {
			// remember mouse button state
			wm_mouse_button_left_semaphore = TRUE;

			// select object under cursor position
			wm_object_selected = wm_object_find( mouse.x, mouse.y, FALSE );

			// if cursor over selected object is in place of possible header
			if( wm_object_selected -> descriptor -> y > wm_object_selected -> descriptor -> offset && wm_object_selected -> descriptor -> y - wm_object_selected -> descriptor -> offset < LIB_INTERFACE_HEADER_HEIGHT_pixel && wm_object_selected -> descriptor -> x < wm_object_selected -> width - ((LIB_INTERFACE_HEADER_HEIGHT_pixel * 0x03) + wm_object_selected -> descriptor -> offset) ) wm_object_drag_semaphore = TRUE;

			// check if object can be moved along Z axis
			if( ! (wm_object_selected -> descriptor -> flags & STD_WINDOW_FLAG_fixed_z) && ! wm_keyboard_status_alt_left ) {
				// move object up inside list
				if( wm_object_move_up( wm_object_selected ) ) {
					// redraw object
					wm_object_selected -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

					// cursor pointer may be obscured, redraw
					wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
				}
			}

			// if left ALT key is not holded
			if( ! wm_keyboard_status_alt_left ) {
				// selected object is menu?
				if( wm_object_selected == wm_object_menu ) {
					// check incomming interface events
					lib_interface_event( (struct LIB_INTERFACE_STRUCTURE *) &menu_interface, TRUE );
				}

				// make object as active if not a taskbar
				if( ! (wm_object_selected -> descriptor -> flags & STD_WINDOW_FLAG_taskbar) ) wm_object_active = wm_object_selected;

				// properties of mouse message
				struct STD_IPC_STRUCTURE_MOUSE *mouse = (struct STD_IPC_STRUCTURE_MOUSE *) &data;

				// default values
				mouse -> ipc.type = STD_IPC_TYPE_mouse;
				mouse -> scroll = EMPTY;

				// left mouse button pressed
				mouse -> button = STD_IPC_MOUSE_BUTTON_left;

				// send event to selected object process
				std_ipc_send( wm_object_selected -> pid, (uint8_t *) mouse );

				// update taskbar status
				wm_taskbar_modified = TRUE;

				// hide unstable objects
				for( uint64_t i = 0; i < wm_list_limit; i++ ) {
					// unstable object?
					if( ! (wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_unstable) ) continue;	// no
					
					// don't show anymore
					wm_list_base_address[ i ] -> descriptor -> flags &= ~STD_WINDOW_FLAG_visible;

					// and redraw area behind
					wm_list_base_address[ i ] -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
				}
			}

			// substitute of menu
			//--------------------
			// if left ALT key is not holded
			if( ! wm_keyboard_status_alt_left )
				// menu button click?
				if( mouse.x < (wm_object_taskbar -> x + WM_OBJECT_TASKBAR_HEIGHT_pixel) && mouse.y >= wm_object_taskbar -> y )
					// generate and show menu window
					wm_object_menu -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;
		}
	} else {
		// left mouse button was held?
		if( wm_mouse_button_left_semaphore ) {
			// properties of mouse message
			struct STD_IPC_STRUCTURE_MOUSE *mouse = (struct STD_IPC_STRUCTURE_MOUSE *) &data;

			// default values
			mouse -> ipc.type = STD_IPC_TYPE_mouse;
			mouse -> scroll = EMPTY;

			// left mouse button released
			mouse -> button = ~STD_IPC_MOUSE_BUTTON_left;

			// send event to selected object process
			std_ipc_send( wm_object_selected -> pid, (uint8_t *) mouse );
		}

		// release mouse button state
		wm_mouse_button_left_semaphore = FALSE;

		// by default object is not draggable
		wm_object_drag_semaphore = FALSE;
	}

	//--------------------------------------------------------------------------

	// right mouse button pressed?
	if( mouse.status & STD_IPC_MOUSE_BUTTON_right ) {
		// left ALT key is in hold
		if( ! wm_object_hover_semaphore ) {
			// first initialization executed
			wm_object_hover_semaphore = TRUE;

			// find object under cursor position
			struct WM_STRUCTURE_OBJECT *wm_object_modify = wm_object_find( mouse.x, mouse.y, FALSE );

			// object resizable?
			if( wm_object_modify -> descriptor -> flags & STD_WINDOW_FLAG_resizable ) {
				// select zone modification

				// by default
				wm_zone_modify.width = TRUE;
				wm_zone_modify.height = TRUE;

				// X axis
				if( wm_object_modify -> descriptor -> x < (wm_object_modify -> width >> STD_SHIFT_2) ) wm_zone_modify.x = TRUE;	// yes
				else wm_zone_modify.x = FALSE;	// no

				// Y axis
				if( wm_object_modify -> descriptor -> y < (wm_object_modify -> height >> STD_SHIFT_2) ) wm_zone_modify.y = TRUE;	// yes
				else wm_zone_modify.y = FALSE;	// no

				// initialize hover object

				// create initial resize object
				wm_object_hover = wm_object_create( wm_object_modify -> x, wm_object_modify -> y, wm_object_modify -> width, wm_object_modify -> height );

				// mark it as our
				wm_object_hover -> pid = wm_pid;

				// fill object with default pattern/color
				uint32_t *hover_pixel = (uint32_t *) ((uintptr_t) wm_object_hover -> descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ));
				for( uint16_t y = 0; y < wm_object_hover -> height; y++ )
					for( uint16_t x = 0; x < wm_object_hover -> width; x++ )
						hover_pixel[ (y * wm_object_hover -> width) + x ] = 0x20008000;

				// show object
				wm_object_hover -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;
			}
		}
	} else {
		// stop hover phase
		wm_object_hover_semaphore = FALSE;

		// if enabled
		if( wm_object_hover && wm_object_hover -> descriptor ) {
			// remove object
			wm_object_hover -> descriptor -> flags = STD_WINDOW_FLAG_release;

			// relese pointer
			wm_object_hover = EMPTY;
		}
	}

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

		// if object selected and left mouse button is held with left alt key
		if( wm_object_drag_semaphore || (wm_object_selected && wm_mouse_button_left_semaphore && wm_keyboard_status_alt_left) )
			// move object along with cursor pointer
			wm_object_move( delta_x, delta_y );

		// if hover object is created
		if( wm_object_hover_semaphore && wm_object_hover ) {
			// remove old instance from cache
			wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm_object_hover, FALSE );

			// block access to object array
			MACRO_LOCK( wm_list_semaphore );

			// block access to object array
			MACRO_LOCK( wm_object_semaphore );

			// left zone?
			if( wm_zone_modify.x && wm_zone_modify.width ) {
				// do not move hover zone
				if( wm_object_hover -> x + delta_x < wm_object_hover -> x + wm_object_hover -> width ) {
					wm_object_hover -> x += delta_x;
					wm_object_hover -> width -= delta_x;
				}
			} else wm_object_hover -> width += delta_x;	// right

			// up zone?
			if( wm_zone_modify.y && wm_zone_modify.height ) {
				// do not move hover zone
				if( wm_object_hover -> y + delta_y < wm_object_hover -> y + wm_object_hover -> height ) {
					wm_object_hover -> y += delta_y;
					wm_object_hover -> height -= delta_y;
				}
			} else wm_object_hover -> height += delta_y;	// down

			// do not allow object width/height less than 1 pixel
			if( wm_object_hover -> width < TRUE ) wm_object_hover -> width = TRUE;
			if( wm_object_hover -> height < TRUE ) wm_object_hover -> height = TRUE;

			// release old object area
			std_memory_release( (uintptr_t) wm_object_hover -> descriptor, MACRO_PAGE_ALIGN_UP( wm_object_hover -> size_byte ) >> STD_SHIFT_PAGE );

			// calculate new object area size in Bytes
			wm_object_hover -> size_byte = ( wm_object_hover -> width * wm_object_hover -> height * STD_VIDEO_DEPTH_byte) + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR );

			// assign new area for object
			wm_object_hover -> descriptor = (struct STD_WINDOW_STRUCTURE_DESCRIPTOR *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( wm_object_hover -> size_byte ) >> STD_SHIFT_PAGE );

			// fill object with default pattern/color
			uint32_t *hover_pixel = (uint32_t *) ((uintptr_t) wm_object_hover -> descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ));
			for( uint16_t y = 0; y < wm_object_hover -> height; y++ )
				for( uint16_t x = 0; x < wm_object_hover -> width; x++ )
					hover_pixel[ (y * wm_object_hover -> width) + x ] = 0x20008000;

			// release access to object array
			MACRO_UNLOCK( wm_object_semaphore );

			// release access to object array
			MACRO_UNLOCK( wm_list_semaphore );

			// show object
			wm_object_hover -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;
		}
	}
}
