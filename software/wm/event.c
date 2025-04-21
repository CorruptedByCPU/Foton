/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_event( void ) {
	// retrieve current mouse status and position
	struct STD_STRUCTURE_MOUSE_SYSCALL mouse_syscall;
	std_mouse( (struct STD_STRUCTURE_MOUSE_SYSCALL *) &mouse_syscall );

	// incomming message
	uint8_t data[ STD_IPC_SIZE_byte ]; int64_t source = EMPTY;
	while( (source = std_ipc_receive_by_type( (uint8_t *) &data, STD_IPC_TYPE_event )) ) {
		MACRO_DEBUF();

		// properties of request
		struct STD_STRUCTURE_IPC_WINDOW *request = (struct STD_STRUCTURE_IPC_WINDOW *) &data;

		// properties of answer
		struct STD_STRUCTURE_IPC_WINDOW_DESCRIPTOR *answer = (struct STD_STRUCTURE_IPC_WINDOW_DESCRIPTOR *) &data;

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

	// incomming mouse event
	// while( (source = std_ipc_receive_by_type( (uint8_t *) &data, STD_IPC_TYPE_mouse )) ) {
	// 	// ignore if workbench locked
	// 	if( wm_object_lock -> descriptor -> flags & STD_WINDOW_FLAG_visible ) break;

	// 	// properties of mouse event
	// 	struct STD_STRUCTURE_IPC_MOUSE *mouse = (struct STD_STRUCTURE_IPC_MOUSE *) &data;

	// 	// different button than required?
	// 	if( ! wm_mouse_button_left_semaphore || ! (mouse -> button & STD_IPC_MOUSE_BUTTON_left) ) continue;	// yes

	// 	// select object under cursor position
	// 	struct WM_STRUCTURE_OBJECT *object = wm_object_find( mouse_syscall.x, mouse_syscall.y, FALSE );

	// 	// thats an icon object?
	// 	if( object -> descriptor -> flags & STD_WINDOW_FLAG_icon )
	// 		// execute command inside object name
	// 		std_exec( (uint8_t *) object -> descriptor -> name, object -> descriptor -> name_length, EMPTY, TRUE );
	// }

	// check keyboard cache
	uint16_t key = std_keyboard();

	// incomming key?
	if( key ) {
		// properties of keyboard message
		struct STD_STRUCTURE_IPC_KEYBOARD *keyboard = (struct STD_STRUCTURE_IPC_KEYBOARD *) &data;

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

			// menu pressed
			case STD_KEY_MENU: {
				// remember menu state
				wm_keyboard_status_menu = TRUE;

				// ignore key
				send = FALSE;

				// done
				break;
			}

	// 		case STD_ASCII_LETTER_l: {
	// 			// menu key on hold?
	// 			if( wm_keyboard_status_menu ) {
	// 				// lock workbench
	// 				wm_object_lock -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;
	//
	// 				// set new active object
	// 				wm_object_active = wm_object_lock;
	//
	// 				// update taskbar status
	// 				wm_taskbar_modified = TRUE;
	//
	// 				// ignore key
	// 				send = FALSE;
	// 			}
	//
	// 			// done
	// 			break;
	// 		}

			// menu released
			case STD_KEY_MENU | 0x80: {
	// 			// ignore if workbench locked
	// 			if( wm_object_lock -> descriptor -> flags & STD_WINDOW_FLAG_visible ) break;
	//
				// show/hide menu
				// if( wm_keyboard_status_menu ) wm_menu_switch( FALSE );
	
				// remember menu state
				wm_keyboard_status_menu = FALSE;
				
				// ignore key
				send = FALSE;

				// done
				break;
			}

			// tab pressed
			case STD_KEY_TAB: {
				// ignore if workbench locked
				// if( wm_object_lock -> descriptor -> flags & STD_WINDOW_FLAG_visible ) break;

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

				// ignore key
				send = FALSE;

				// done
				break;
			}

	// 		// return pressed
	// 		case STD_ASCII_RETURN: {
	// 			// ignore if workbench locked
	// 			if( wm_object_lock -> descriptor -> flags & STD_WINDOW_FLAG_visible ) break;

	// 			// alt key is on hold
	// 			if( wm_keyboard_status_alt_left ) {
	// 				// execute console application
	// 				std_exec( (uint8_t *) "console", 7, EMPTY, TRUE );

	// 				// do not send this key to current process
	// 				send = FALSE;
	// 			}

	// 			// done
	// 			break;
	// 		}
		}

		// send event to active object process
		if( send ) std_ipc_send( wm_object_active -> pid, (uint8_t *) keyboard );
	}

	// calculate delta of cursor new position
	int16_t delta_x = mouse_syscall.x - wm_object_cursor -> x;
	int16_t delta_y = mouse_syscall.y - wm_object_cursor -> y;
	int16_t delta_z = mouse_syscall.z - wm_mouse_z; wm_mouse_z = mouse_syscall.z;

	//--------------------------------------------------------------------------

	// select object under cursor position
	struct WM_STRUCTURE_OBJECT *object = wm_object_find( mouse_syscall.x, mouse_syscall.y, FALSE );

	// update pointer position
	object -> descriptor -> x = (wm_object_cursor -> x + delta_x) - object -> x;
	object -> descriptor -> y = (wm_object_cursor -> y + delta_y) - object -> y;

	//--------------------------------------------------------------------------

	// left mouse button pressed?
	if( mouse_syscall.status & STD_MOUSE_BUTTON_left ) {
		// isn't holded down?
		if( ! wm_mouse_button_left_semaphore ) {
			// remember mouse button state
			wm_mouse_button_left_semaphore = TRUE;

			// select object under cursor position
			wm_object_selected = wm_object_find( mouse_syscall.x, mouse_syscall.y, FALSE );

			// if cursor over selected object is in place of possible header
			if( wm_object_selected -> descriptor -> y > 0 && wm_object_selected -> descriptor -> y < LIB_INTERFACE_HEADER_HEIGHT_pixel && wm_object_selected -> descriptor -> x < wm_object_selected -> width - ((LIB_INTERFACE_HEADER_HEIGHT_pixel * 0x03)) ) wm_object_drag_semaphore = TRUE;

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

			// if Menu key is not on hold
	//		if( ! wm_keyboard_status_menu ) {
	// 			// selected object is menu?
	// 			if( wm_object_selected == wm_object_menu )
	// 				// check incomming interface events
	// 				lib_interface_event_handler_release( (struct LIB_INTERFACE_STRUCTURE *) &menu_interface );

	// 			// make object as active if not a taskbar or icon
	// 			if( ! (wm_object_selected -> descriptor -> flags & (STD_WINDOW_FLAG_taskbar | STD_WINDOW_FLAG_icon)) ) wm_object_active = wm_object_selected;

	// 			// properties of mouse message
	// 			struct STD_STRUCTURE_IPC_MOUSE *mouse = (struct STD_STRUCTURE_IPC_MOUSE *) &data;

	// 			// default values
	// 			mouse -> ipc.type = STD_IPC_TYPE_mouse;
	// 			mouse -> scroll = EMPTY;

	// 			// left mouse button pressed
	// 			mouse -> button = STD_IPC_MOUSE_BUTTON_left;

	// 			// send event to selected object process
	// 			std_ipc_send( wm_object_selected -> pid, (uint8_t *) mouse );

	// 			// update taskbar status
	// 			wm_taskbar_modified = TRUE;

	// 			// hide unstable objects
	// 			for( uint64_t i = 0; i < wm_list_limit; i++ ) {
	// 				// unstable object?
	// 				if( ! (wm_list_base_address[ i ] -> descriptor -> flags & STD_WINDOW_FLAG_unstable) ) continue;	// no
					
	// 				// don't show anymore
	// 				wm_list_base_address[ i ] -> descriptor -> flags &= ~STD_WINDOW_FLAG_visible;

	// 				// and redraw area behind
	// 				wm_list_base_address[ i ] -> descriptor -> flags |= STD_WINDOW_FLAG_flush;
	// 			}

	// 			// substitute of menu
	// 			//--------------------

	// 			// menu button click?
	// 			if( mouse_syscall.x < (wm_object_taskbar -> x + WM_OBJECT_TASKBAR_HEIGHT_pixel) && mouse_syscall.y >= wm_object_taskbar -> y )
	// 				// ignore if workbench locked
	// 				if( ! (wm_object_lock -> descriptor -> flags & STD_WINDOW_FLAG_visible) )
	// 					// show/hide menu window
	// 					wm_menu_switch( FALSE );
	//		}
		}
	} else {
		// left mouse button was held?
		if( wm_mouse_button_left_semaphore ) {
			// properties of mouse message
			struct STD_STRUCTURE_IPC_MOUSE *mouse = (struct STD_STRUCTURE_IPC_MOUSE *) &data;
	
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
	if( mouse_syscall.status & STD_IPC_MOUSE_BUTTON_right ) {
		// Menu key is on hold
		if( wm_keyboard_status_alt_left && ! wm_object_hover_semaphore ) {
			// first initialization executed
			wm_object_hover_semaphore = TRUE;

			// find object under cursor position
			wm_object_modify = wm_object_find( mouse_syscall.x, mouse_syscall.y, FALSE );

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
				uint32_t *hover_pixel = (uint32_t *) ((uintptr_t) wm_object_hover -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ));
				for( uint16_t y = 0; y < wm_object_hover -> height; y++ )
					for( uint16_t x = 0; x < wm_object_hover -> width; x++ )
						hover_pixel[ (y * wm_object_hover -> width) + x ] = 0x20008000;

				// and point border
				for( uint16_t y = 0; y < wm_object_hover -> height; y++ )
					for( uint16_t x = 0; x < wm_object_hover -> width; x++ )
						if( ! x || ! y || x == wm_object_hover -> width - 1 || y == wm_object_hover -> height - 1 ) hover_pixel[ (y * wm_object_hover -> width) + x ] = 0x80008000;

				// show object
				wm_object_hover -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;
			}
		}
	} else {
		// stop hover phase
		wm_object_hover_semaphore = FALSE;

		// if enabled
		if( wm_object_hover && wm_object_hover -> descriptor ) {
			// copy hover object properties to selected object
			wm_object_modify -> descriptor -> new_x		= wm_object_hover -> x;
			wm_object_modify -> descriptor -> new_y		= wm_object_hover -> y;
			wm_object_modify -> descriptor -> new_width	= wm_object_hover -> width;
			wm_object_modify -> descriptor -> new_height	= wm_object_hover -> height;

			// inform application interface about requested properties
			wm_object_modify -> descriptor -> flags |= STD_WINDOW_FLAG_properties;

			// remove hover object
			wm_object_hover -> descriptor -> flags = STD_WINDOW_FLAG_release;

			// relese pointer
			wm_object_hover = EMPTY;
		}
	}

	// scroll movement?
	if( delta_z ) {
		// properties of mouse message
		struct STD_STRUCTURE_IPC_MOUSE *mouse = (struct STD_STRUCTURE_IPC_MOUSE *) &data;

		// default values
		mouse -> ipc.type = STD_IPC_TYPE_mouse;
		mouse -> scroll = delta_z;

		// select object under cursor position
		struct WM_STRUCTURE_OBJECT *object = wm_object_find( mouse_syscall.x, mouse_syscall.y, FALSE );

		// send event to selected object process
		std_ipc_send( object -> pid, (uint8_t *) mouse );
	}

	//--------------------------------------------------------------------------
	// if cursor pointer movement occurs
	if( delta_x || delta_y ) {
		// remove current cursor position from workbench
		wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm_object_cursor, FALSE );

		// update cursor position
		wm_object_cursor -> x = mouse_syscall.x;
		wm_object_cursor -> y = mouse_syscall.y;

		// redisplay cursor at new location
		wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

		// if object selected and left mouse button is held with Menu key
		if( wm_object_drag_semaphore || (wm_object_selected && wm_mouse_button_left_semaphore && wm_keyboard_status_alt_left) )
			// move object along with cursor pointer
			wm_object_move( delta_x, delta_y );

		// if hover object is created
		if( wm_object_hover_semaphore && wm_object_hover ) {
			// block access to object array
			MACRO_LOCK( wm_list_semaphore );

			// block access to object array
			MACRO_LOCK( wm_object_semaphore );

			// remove old instance from cache
			wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm_object_hover, FALSE );

			// release old object area
			std_memory_release( (uintptr_t) wm_object_hover -> descriptor, MACRO_PAGE_ALIGN_UP( wm_object_hover -> size_byte ) >> STD_SHIFT_PAGE );

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

			// and larger than current screen properties
			if( wm_object_hover -> width > wm_object_workbench -> width ) wm_object_hover -> width = wm_object_workbench -> width;
			if( wm_object_hover -> height > wm_object_workbench -> height ) wm_object_hover -> height = wm_object_workbench -> height;

			// calculate new object area size in Bytes
			wm_object_hover -> size_byte = ( wm_object_hover -> width * wm_object_hover -> height * STD_VIDEO_DEPTH_byte) + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR );

			// assign new area for object
			wm_object_hover -> descriptor = (struct STD_STRUCTURE_WINDOW_DESCRIPTOR *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( wm_object_hover -> size_byte ) >> STD_SHIFT_PAGE );

			// fill object with default pattern/color
			uint32_t *hover_pixel = (uint32_t *) ((uintptr_t) wm_object_hover -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ));
			for( uint16_t y = 0; y < wm_object_hover -> height; y++ )
				for( uint16_t x = 0; x < wm_object_hover -> width; x++ )
					hover_pixel[ (y * wm_object_hover -> width) + x ] = 0x10008000;

			// and point border
			for( uint16_t y = 0; y < wm_object_hover -> height; y++ )
				for( uint16_t x = 0; x < wm_object_hover -> width; x++ )
					if( ! x || ! y || x == wm_object_hover -> width - 1 || y == wm_object_hover -> height - 1 ) hover_pixel[ (y * wm_object_hover -> width) + x ] = 0x80008000;

			// release access to object array
			MACRO_UNLOCK( wm_object_semaphore );

			// release access to object array
			MACRO_UNLOCK( wm_list_semaphore );

			// show object
			wm_object_hover -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;
		}
	}

	// // Hmmm....
	// // check events from keyboard
	// lib_interface_event_keyboard( (struct LIB_INTERFACE_STRUCTURE *) &menu_interface );
}
