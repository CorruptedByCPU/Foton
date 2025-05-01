/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void tiwyn_event_shade_fill( void ) {
	// fill object with default pattern/color
	uint32_t *shade = (uint32_t *) ((uintptr_t) tiwyn -> shade -> descriptor + sizeof( struct LIB_WINDOW_DESCRIPTOR ));
	for( uint16_t y = 0; y < tiwyn -> shade -> height; y++ )
		for( uint16_t x = 0; x < tiwyn -> shade -> width; x++ )
			shade[ (y * tiwyn -> shade -> width) + x ] = TIWYN_OBJECT_OVERSHADE_COLOR;

	// and border
	for( uint16_t y = 0; y < tiwyn -> shade -> height; y++ )
		for( uint16_t x = 0; x < tiwyn -> shade -> width; x++ )
			if( ! x || ! y || x == tiwyn -> shade -> width - 1 || y == tiwyn -> shade -> height - 1 ) shade[ (y * tiwyn -> shade -> width) + x ] = TIWYN_OBJECT_OVERSHADE_COLOR_BORDER;
}

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

	// create shade object?
	if( (mouse.status & STD_MOUSE_BUTTON_right) ) { if( ! tiwyn -> mouse_button_right ) { if( tiwyn -> key_ctrl_left && ! tiwyn -> shade_initialized && current -> descriptor -> flags & STD_WINDOW_FLAG_resizable ) {
		// current object under cursor pointer selected for resize
		tiwyn -> resized = current;

		//--------------------------------------------------------------

		// by default, user holds right or bottom side
		tiwyn -> direction.width = TRUE;
		tiwyn -> direction.height = TRUE;

		// is it left side?
		if( tiwyn -> resized -> descriptor -> x < (tiwyn -> resized -> width >> STD_SHIFT_2) ) tiwyn -> direction.x = TRUE;	// yes
		else tiwyn -> direction.x = FALSE;	// no

		// is it top side?
		if( tiwyn -> resized -> descriptor -> y < (tiwyn -> resized -> height >> STD_SHIFT_2) ) tiwyn -> direction.y = TRUE;	// yes
		else tiwyn -> direction.y = FALSE;	// no

		//--------------------------------------------------------------

		// create initial shade object
		if( (tiwyn -> shade = tiwyn_object_create( tiwyn -> resized -> x, tiwyn -> resized -> y, tiwyn -> resized -> width, tiwyn -> resized -> height )) ) {
			// set gradient
			tiwyn_event_shade_fill();

			// show object
			tiwyn -> shade -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;	

			// done
			tiwyn -> shade_initialized = TRUE;
		}
	} } } else {
		// if shade object exist
		if( tiwyn -> shade && tiwyn -> shade -> descriptor ) {
			// copy hover object properties to selected object
			tiwyn -> resized -> descriptor -> new_x		= tiwyn -> shade -> x;
			tiwyn -> resized -> descriptor -> new_y		= tiwyn -> shade -> y;
			tiwyn -> resized -> descriptor -> new_width	= tiwyn -> shade -> width;
			tiwyn -> resized -> descriptor -> new_height	= tiwyn -> shade -> height;

			// inform application interface about requested properties
			tiwyn -> resized -> descriptor -> flags |= STD_WINDOW_FLAG_properties;

			// remove shade object
			tiwyn -> shade -> descriptor -> flags = STD_WINDOW_FLAG_release;

			// debug
			tiwyn -> shade = EMPTY;
		}

		// uninitialize shade
		tiwyn -> shade_initialized = FALSE;

		// release mouse button state
		tiwyn -> mouse_button_right = FALSE;
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

	//----------------------------------------------------------------------

	// if shade object doesn't exist
	if( ! tiwyn -> shade_initialized ) return;	// done

	// remove object visualization
	tiwyn_zone_insert( (struct TIWYN_STRUCTURE_ZONE *) tiwyn -> shade, FALSE );

	// release object content
	std_memory_release( (uintptr_t) tiwyn -> shade -> descriptor, MACRO_PAGE_ALIGN_UP( tiwyn -> shade -> limit ) >> STD_SHIFT_PAGE );

	//----------------------------------------------------------------------

	// which part of shade should be changed

	// left zone?
	if( tiwyn -> direction.x && tiwyn -> direction.width ) {
		// do not move hover zone
		if( tiwyn -> shade -> x + delta_x < tiwyn -> shade -> x + tiwyn -> shade -> width ) {
			tiwyn -> shade -> x += delta_x;
			tiwyn -> shade -> width -= delta_x;
		}
	} else tiwyn -> shade -> width += delta_x;	// right

	// up zone?
	if( tiwyn -> direction.y && tiwyn -> direction.height ) {
		// do not move hover zone
		if( tiwyn -> shade -> y + delta_y < tiwyn -> shade -> y + tiwyn -> shade -> height ) {
			tiwyn -> shade -> y += delta_y;
			tiwyn -> shade -> height -= delta_y;
		}
	} else tiwyn -> shade -> height += delta_y;	// down

	// do not allow object width/height less than 1 pixel
	if( tiwyn -> shade -> width < TRUE ) tiwyn -> shade -> width = TRUE;
	if( tiwyn -> shade -> height < TRUE ) tiwyn -> shade -> height = TRUE;

	// and larger than current screen properties
	if( tiwyn -> shade -> width > tiwyn -> canvas.width ) tiwyn -> shade -> width = tiwyn -> canvas.width;
	if( tiwyn -> shade -> height > tiwyn -> canvas.height ) tiwyn -> shade -> height = tiwyn -> canvas.height;

	//----------------------------------------------------------------------

	// calculate new object area size in Bytes
	tiwyn -> shade -> limit = ((tiwyn -> shade -> width * tiwyn -> shade -> height) << STD_VIDEO_DEPTH_shift) + sizeof( struct LIB_WINDOW_DESCRIPTOR );

	// assign new area for object
	tiwyn -> shade -> descriptor = (struct LIB_WINDOW_DESCRIPTOR *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( tiwyn -> shade -> limit ) >> STD_SHIFT_PAGE ); tiwyn_event_shade_fill();

	// show refresh object content
	tiwyn -> shade -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;
}
