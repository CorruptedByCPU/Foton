/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_event_shade_fill( void ) {
	// fill object with default pattern/color
	uint32_t *shade = (uint32_t *) ((uintptr_t) wm -> shade -> descriptor + sizeof( struct LIB_WINDOW_STRUCTURE_DESCRIPTOR ));
	for( uint16_t y = 0; y < wm -> shade -> height; y++ )
		for( uint16_t x = 0; x < wm -> shade -> width; x++ )
			shade[ (y * wm -> shade -> width) + x ] = WM_OBJECT_OVERSHADE_COLOR;

	// and border
	for( uint16_t y = 0; y < wm -> shade -> height; y++ )
		for( uint16_t x = 0; x < wm -> shade -> width; x++ )
			if( ! x || ! y || x == wm -> shade -> width - 1 || y == wm -> shade -> height - 1 ) shade[ (y * wm -> shade -> width) + x ] = WM_OBJECT_OVERSHADE_COLOR_BORDER;
}

void wm_event( void ) {
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
				struct WM_STRUCTURE_OBJECT *new = wm_object_create( create -> x, create -> y, create -> width, create -> height );

				// by default, couldn't create object
				answer -> descriptor = EMPTY;

				// if created properly, try to share object descriptor with process
				if( new ) answer -> descriptor = std_memory_share( pid, (uintptr_t) new -> descriptor, MACRO_PAGE_ALIGN_UP( new -> limit ) >> STD_SHIFT_PAGE, TRUE );

				// if everything was done properly
				if( answer -> descriptor ) new -> pid = pid;

				// send answer
				std_ipc_send( pid, (uint8_t *) answer );

				// newly created object becomes active
				wm -> active = new; wm -> active -> descriptor -> flags |= LIB_WINDOW_FLAG_active;
				
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
	struct WM_STRUCTURE_OBJECT *current = wm_object_find( mouse.x, mouse.y, FALSE );

	// calculate delta of cursor new position
	int16_t delta_x = mouse.x - wm -> cursor -> x;
	int16_t delta_y = mouse.y - wm -> cursor -> y;
	int16_t delta_z = mouse.z - wm -> cursor -> z;

	// update pointer position inside current object
	current -> descriptor -> x = (wm -> cursor -> x + delta_x) - current -> x;
	current -> descriptor -> y = (wm -> cursor -> y + delta_y) - current -> y;
	current -> descriptor -> z = delta_z;

	//--------------------------------------------------------------

	// check keyboard cache
	uint16_t key = std_keyboard();

	// remember state of special key, or take action immediately
	switch( key ) {
		// left ctrl pressed
		case STD_KEY_CTRL_LEFT: { wm -> key_ctrl_left = TRUE; break; }

		// left ctrl released
		case STD_KEY_CTRL_LEFT | 0x80: { wm -> key_ctrl_left = FALSE; break; }
	}

	//--------------------------------------------------------------

	// left mouse button pressed? and not on hold
	if( (mouse.status & STD_MOUSE_BUTTON_left) ) { if( ! wm -> mouse_button_left ) {
		// remember mouse button state
		wm -> mouse_button_left = TRUE;

		// current object under cursor pointer set as selected
		wm -> selected = current;

		// cursor in position of object header
		if( wm -> selected -> descriptor -> y < wm -> selected -> descriptor -> header_height ) wm -> drag_allow = TRUE;

		// can we move object on top of object list?
		if( ! (wm -> selected -> descriptor -> flags & LIB_WINDOW_FLAG_fixed_z) && ! wm -> key_ctrl_left && wm_object_move_up( wm -> selected ) )
			// object moved on top, redraw
			wm -> selected -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;

		// set object as active?
		if( ! wm -> key_ctrl_left && wm -> selected != wm -> panel ) { wm -> active = wm -> selected; wm -> active -> descriptor -> flags |= LIB_WINDOW_FLAG_active; }

		//--------------------------------------------------------------

		// mouse pointer in range of any list entry of panel?
		if( wm -> cursor -> y >= wm -> panel -> y && (wm -> cursor -> x >= WM_PANEL_HEIGHT_pixel && wm -> cursor -> x < (WM_PANEL_HEIGHT_pixel + (wm -> panel_entry_width * wm -> list_limit_panel))) ) {
			// no action on empty panel
			if( wm -> list_limit_panel ) {
				// properties of selected object entry
				struct WM_STRUCTURE_OBJECT *object = wm -> list_panel[ (wm -> panel -> descriptor -> x - WM_PANEL_HEIGHT_pixel) / wm -> panel_entry_width ];

				// active?
				if( object == wm -> active )
					// remove object from cache
					object -> descriptor -> flags |= LIB_WINDOW_FLAG_hide;
				else {
					// set object on top
					wm_object_move_up( object );

					// show object inside cache
					object -> descriptor -> flags |= LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;

					// object is active now
					wm -> active = object; wm -> active -> descriptor -> flags |= LIB_WINDOW_FLAG_active;
				}
			}
		}

		// do not send messages to ourselfs
		if( wm -> selected -> pid != wm -> pid ) {
			// properties of mouse message
			struct STD_STRUCTURE_IPC_MOUSE *mouse = (struct STD_STRUCTURE_IPC_MOUSE *) &ipc_data;

			// default values
			mouse -> ipc.type = STD_IPC_TYPE_mouse;
			mouse -> scroll = EMPTY;

			// left mouse button pressed
			mouse -> button = STD_IPC_MOUSE_BUTTON_left;

			// send event to selected object owner
			std_ipc_send( wm -> selected -> pid, (uint8_t *) mouse );
		}
	} } else {
		// release mouse button state
		wm -> mouse_button_left = FALSE;

		// disable object drag
		wm -> drag_allow = FALSE;
	}

	// create shade object?
	if( (mouse.status & STD_MOUSE_BUTTON_right) ) { if( ! wm -> mouse_button_right ) { if( wm -> key_ctrl_left && ! wm -> shade_initialized && current -> descriptor -> flags & LIB_WINDOW_FLAG_resizable ) {
		// current object under cursor pointer selected for resize
		wm -> resized = current;

		//--------------------------------------------------------------

		// by default, user holds right or bottom side
		wm -> direction.width = TRUE;
		wm -> direction.height = TRUE;

		// is it left side?
		if( wm -> resized -> descriptor -> x < (wm -> resized -> width >> STD_SHIFT_2) ) wm -> direction.x = TRUE;	// yes
		else wm -> direction.x = FALSE;	// no

		// is it top side?
		if( wm -> resized -> descriptor -> y < (wm -> resized -> height >> STD_SHIFT_2) ) wm -> direction.y = TRUE;	// yes
		else wm -> direction.y = FALSE;	// no

		//--------------------------------------------------------------

		// create initial shade object
		if( (wm -> shade = wm_object_create( wm -> resized -> x, wm -> resized -> y, wm -> resized -> width, wm -> resized -> height )) ) {
			// set gradient
			wm_event_shade_fill();

			// show object
			wm -> shade -> descriptor -> flags |= LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;	

			// done
			wm -> shade_initialized = TRUE;
		}
	} } } else {
		// if shade object exist
		if( wm -> shade && wm -> shade -> descriptor ) {
			// copy hover object properties to selected object
			wm -> resized -> descriptor -> new_x		= wm -> shade -> x;
			wm -> resized -> descriptor -> new_y		= wm -> shade -> y;
			wm -> resized -> descriptor -> new_width	= wm -> shade -> width;
			wm -> resized -> descriptor -> new_height	= wm -> shade -> height;

			// inform application interface about requested properties
			wm -> resized -> descriptor -> flags |= LIB_WINDOW_FLAG_properties;

			// remove shade object
			wm -> shade -> descriptor -> flags = LIB_WINDOW_FLAG_release;

			// debug
			wm -> shade = EMPTY;
		}

		// uninitialize shade
		wm -> shade_initialized = FALSE;

		// release mouse button state
		wm -> mouse_button_right = FALSE;
	}

	//--------------------------------------------------------------

	// if cursor pointer movement didn't occur
	if( ! delta_x && ! delta_y ) return;	// done

	// remove current cursor position from workbench
	wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm -> cursor, FALSE );

	// update cursor position
	wm -> cursor -> x = mouse.x;
	wm -> cursor -> y = mouse.y;

	// redisplay cursor at new location
	wm -> cursor -> descriptor -> flags |= LIB_WINDOW_FLAG_flush;

	// move object along with cursor pointer?
	if( wm -> mouse_button_left && (wm -> key_ctrl_left || wm -> drag_allow) && ! (wm -> selected -> descriptor -> flags & LIB_WINDOW_FLAG_fixed_xy) ) wm_object_move( delta_x, delta_y );

	//----------------------------------------------------------------------

	// if shade object doesn't exist
	if( ! wm -> shade_initialized ) return;	// done

	// remove object visualization
	wm_zone_insert( (struct WM_STRUCTURE_ZONE *) wm -> shade, FALSE );

	// release object content
	std_memory_release( (uintptr_t) wm -> shade -> descriptor, MACRO_PAGE_ALIGN_UP( wm -> shade -> limit ) >> STD_SHIFT_PAGE );

	//----------------------------------------------------------------------

	// which part of shade should be changed

	// left zone?
	if( wm -> direction.x && wm -> direction.width ) {
		// do not move hover zone
		if( wm -> shade -> x + delta_x < wm -> shade -> x + wm -> shade -> width ) {
			wm -> shade -> x += delta_x;
			wm -> shade -> width -= delta_x;
		}
	} else wm -> shade -> width += delta_x;	// right

	// up zone?
	if( wm -> direction.y && wm -> direction.height ) {
		// do not move hover zone
		if( wm -> shade -> y + delta_y < wm -> shade -> y + wm -> shade -> height ) {
			wm -> shade -> y += delta_y;
			wm -> shade -> height -= delta_y;
		}
	} else wm -> shade -> height += delta_y;	// down

	// do not allow object width/height less than 1 pixel
	if( wm -> shade -> width < TRUE ) wm -> shade -> width = TRUE;
	if( wm -> shade -> height < TRUE ) wm -> shade -> height = TRUE;

	// and larger than current screen properties
	if( wm -> shade -> width > wm -> canvas.width ) wm -> shade -> width = wm -> canvas.width;
	if( wm -> shade -> height > wm -> canvas.height ) wm -> shade -> height = wm -> canvas.height;

	//----------------------------------------------------------------------

	// calculate new object area size in Bytes
	wm -> shade -> limit = ((wm -> shade -> width * wm -> shade -> height) << STD_VIDEO_DEPTH_shift) + sizeof( struct LIB_WINDOW_STRUCTURE_DESCRIPTOR );

	// assign new area for object
	wm -> shade -> descriptor = (struct LIB_WINDOW_STRUCTURE_DESCRIPTOR *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( wm -> shade -> limit ) >> STD_SHIFT_PAGE ); wm_event_shade_fill();

	// show refresh object content
	wm -> shade -> descriptor -> flags |= LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_flush;
}
