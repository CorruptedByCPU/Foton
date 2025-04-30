/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void tiwyn_event( void ) {
	// incomming/outgoing messages
	uint8_t ipc_data[ STD_IPC_SIZE_byte ];

	// check every incomming request
	uint64_t pid; while( (pid = std_ipc_receive_by_type( (uint8_t *) &ipc_data, STD_IPC_TYPE_default )) ) {
		// properties of request
		struct STD_STRUCTURE_IPC_WINDOW *request = (struct STD_STRUCTURE_IPC_WINDOW *) &ipc_data;

		// choose behavior
		switch( request -> properties ) {
			case STD_IPC_WINDOW_create: {
				// properties of window create request
				struct STD_STRUCTURE_IPC_WINDOW_CREATE *create = (struct STD_STRUCTURE_IPC_WINDOW_CREATE *) &ipc_data;
				
				// invalid request?
				if( ! create -> width || ! create -> height ) break;	// done

				// properties of answer
				struct STD_STRUCTURE_IPC_WINDOW_DESCRIPTOR *answer = (struct STD_STRUCTURE_IPC_WINDOW_DESCRIPTOR *) &ipc_data;

				// properties of new object
				struct TIWYN_STRUCTURE_OBJECT *new = tiwyn_object_create( create -> x, create -> y, create -> width, create -> height );

				// by default, couldn't create object
				answer -> descriptor = EMPTY;

				// if created properly, try to share object descriptor with process
				if( new ) answer -> descriptor = std_memory_share( pid, (uintptr_t) new -> descriptor, MACRO_PAGE_ALIGN_UP( new -> limit ) >> STD_SHIFT_PAGE, TRUE );

				// if everything was done properly
				if( answer -> descriptor )
					// set object owner
					new -> pid = pid;

				// send answer
				std_ipc_send( pid, (uint8_t *) answer );

				// done
				break;
			}
		}
	}

	//----------------------------------------------------------------------

	// retrieve current mouse status and position
	struct STD_STRUCTURE_MOUSE_SYSCALL mouse;
	std_mouse( (struct STD_STRUCTURE_MOUSE_SYSCALL *) &mouse );

	//----------------------------------------------------------------------

	// select object under cursor position
	struct TIWYN_STRUCTURE_OBJECT *current = tiwyn_object_find( mouse.x, mouse.y, FALSE );

	// calculate delta of cursor new position
	int16_t delta_x = mouse.x - tiwyn -> cursor -> x;
	int16_t delta_y = mouse.y - tiwyn -> cursor -> y;
	int16_t delta_z = mouse.z - tiwyn -> cursor -> z;

	// update pointer position inside current object
	current -> descriptor -> x = (tiwyn -> cursor -> x + delta_x) - current -> x;
	current -> descriptor -> y = (tiwyn -> cursor -> y + delta_y) - current -> y;
	current -> descriptor -> z = delta_z;

	//--------------------------------------------------------------

	// check keyboard cache
	uint16_t key = std_keyboard();

	// remember state of special key, or take action immediately
	switch( key ) {
		// left ctrl pressed
		case STD_KEY_CTRL_LEFT: { tiwyn -> key_ctrl_left = TRUE; break; }

		// left ctrl released
		case STD_KEY_CTRL_LEFT | 0x80: { tiwyn -> key_ctrl_left = FALSE; break; }
	}

	//--------------------------------------------------------------

	// left mouse button pressed? and not on hold
	if( (mouse.status & STD_MOUSE_BUTTON_left) ) { if( ! tiwyn -> mouse_button_left ) {
		// remember mouse button state
		tiwyn -> mouse_button_left = TRUE;

		// current object under cursor pointer set as selected
		tiwyn -> selected = current;

		// cursor in position of object header
		if( tiwyn -> selected -> descriptor -> y < tiwyn -> selected -> descriptor -> header_height ) tiwyn -> drag_allow = TRUE;

		// can we move object on top of object list?
		if( ! (tiwyn -> selected -> descriptor -> flags & STD_WINDOW_FLAG_fixed_z) && ! tiwyn -> key_ctrl_left && wm_object_move_up( tiwyn -> selected ) )
			// object moved on top, redraw
			tiwyn -> selected -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

		// make object as active if not a panel
		if( ! (tiwyn -> selected -> descriptor -> flags & STD_WINDOW_FLAG_panel) ) tiwyn -> active = tiwyn -> selected;
	} } else {
		// release mouse button state
		tiwyn -> mouse_button_left = FALSE;

		// disable object drag
		tiwyn -> drag_allow = FALSE;
	}

	// right mouse button pressed? and not on hold
	if( (mouse.status & STD_MOUSE_BUTTON_right) ) { if( ! tiwyn -> mouse_button_right ) {
	}} else {
		
	}

	//--------------------------------------------------------------

	// if cursor pointer movement didn't occur
	if( ! delta_x && ! delta_y ) return;	// done

	// remove current cursor position from workbench
	tiwyn_zone_insert( (struct TIWYN_STRUCTURE_ZONE *) tiwyn -> cursor, FALSE );

	// update cursor position
	tiwyn -> cursor -> x = mouse.x;
	tiwyn -> cursor -> y = mouse.y;

	// redisplay cursor at new location
	tiwyn -> cursor -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

	// move object along with cursor pointer?
	if( tiwyn -> mouse_button_left && (tiwyn -> key_ctrl_left || tiwyn -> drag_allow) && ! (tiwyn -> selected -> descriptor -> flags & STD_WINDOW_FLAG_fixed_xy) ) tiwyn_object_move( delta_x, delta_y );
}
