/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t gui_init( void ) {
	// get out PID number
	gui_pid = std_pid();

	// obtain information about kernel framebuffer
	struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER kernel_framebuffer;
	std_framebuffer( &kernel_framebuffer );

	// remember Window Manager ID
	wm_pid = kernel_framebuffer.pid;

	// store properties of kernels framebuffer
	gui_wallpaper_width	= kernel_framebuffer.width_pixel;
	gui_wallpaper_height	= kernel_framebuffer.height_pixel;

	// allocate wm data container
	uint8_t wm_data[ STD_IPC_SIZE_byte ];

	// prepeare new window request
	struct STD_IPC_STRUCTURE_WINDOW_REQUEST *wm_request = (struct STD_IPC_STRUCTURE_WINDOW_REQUEST *) &wm_data;
	struct STD_IPC_STRUCTURE_WINDOW_ANSWER_DESCRIPTOR *wm_answer = EMPTY;	// answer will be in here

	//----------------------------------------------------------------------

	// wallpaper window properties
	wm_request -> type = STD_IPC_TYPE_internal;
	wm_request -> request_or_answer = STD_WINDOW_REQUEST_create;
	wm_request -> x = 0;
	wm_request -> y = 0;
	wm_request -> width = gui_wallpaper_width;
	wm_request -> height = gui_wallpaper_height;

	// send request to Window Manager
	std_ipc_send( wm_pid, (uint8_t *) wm_request );

	// wait for answer
	while( ! std_ipc_receive( (uint8_t *) wm_data ) );

	// window assigned?
	wm_answer = (struct STD_IPC_STRUCTURE_WINDOW_ANSWER_DESCRIPTOR *) &wm_data;
	if( ! wm_answer -> descriptor ) return FALSE;	// no

	// properties of wallpaper window
	gui_window_wallpaper = (struct STD_WINDOW_STRUCTURE_DESCRIPTOR *) wm_answer -> descriptor;

	// properties of assigned image to wallpaper window
	struct LIB_IMAGE_TGA_STRUCTURE *image_wallpaper = (struct LIB_IMAGE_TGA_STRUCTURE *) &file_wallpaper_start;

	// convert image to RGBA
	uint32_t *tmp_wallpaper = (uint32_t *) malloc( gui_wallpaper_width * gui_wallpaper_height * STD_VIDEO_DEPTH_byte );
	lib_image_tga_parse( (uint8_t *) image_wallpaper, tmp_wallpaper, (uint64_t) file_wallpaper_end - (uint64_t) file_wallpaper_start );

	// copy scaled image content to wallpaper window
	uint32_t *pixel_wallpaper = (uint32_t *) ((uintptr_t) gui_window_wallpaper + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ));
	float x_scale_factor = (float) ((float) image_wallpaper -> width / (float) gui_wallpaper_width);
	float y_scale_factor = (float) ((float) image_wallpaper -> height / (float) gui_wallpaper_height);
	for( uint16_t y = 0; y < gui_wallpaper_height; y++ )
		for( uint16_t x = 0; x < gui_wallpaper_width; x++ )
			pixel_wallpaper[ (y * gui_wallpaper_width) + x ] = tmp_wallpaper[ (uint64_t) (((uint64_t) (y_scale_factor * y) * image_wallpaper -> width) + (uint64_t) (x * x_scale_factor)) ];

	// release temporary image
	free( tmp_wallpaper );

	// window content ready for display
	gui_window_wallpaper -> flags |= STD_WINDOW_FLAG_fixed_z | STD_WINDOW_FLAG_fixed_xy | STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	//----------------------------------------------------------------------

	// taskbar window properties
	wm_request -> type = STD_IPC_TYPE_internal;
	wm_request -> request_or_answer = STD_WINDOW_REQUEST_create;
	wm_request -> x = 0;
	wm_request -> y = gui_wallpaper_height - GUI_WINDOW_TASKBAR_HEIGHT_pixel;
	wm_request -> width = gui_wallpaper_width;
	wm_request -> height = GUI_WINDOW_TASKBAR_HEIGHT_pixel;

	// send request to Window Manager
	std_ipc_send( wm_pid, (uint8_t *) wm_request );

	// wait for answer0x403010
	while( ! std_ipc_receive( (uint8_t *) wm_data ) );

	// window assigned?
	wm_answer = (struct STD_IPC_STRUCTURE_WINDOW_ANSWER_DESCRIPTOR *) &wm_data;
	if( ! wm_answer -> descriptor ) return FALSE;	// no

	// properties of taskbar window
	gui_window_taskbar = (struct STD_WINDOW_STRUCTURE_DESCRIPTOR *) wm_answer -> descriptor;

	// by default Window Manager creates fully transparent windows and we leave it in that state

// //debug
// uint32_t *pixel_taskbar = (uint32_t *) ((uintptr_t) gui_window_taskbar + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ));
// for( uint16_t y = 0; y < GUI_WINDOW_TASKBAR_HEIGHT_pixel; y++ )
// 	for( uint16_t x = 0; x < gui_wallpaper_width; x++ )
// 		pixel_taskbar[ (y * gui_wallpaper_width) + x ] = STD_COLOR_GREEN_light;

	// mark window as taskbar, so Window Manager will treat it as boundary for other windows
	gui_window_taskbar -> flags |= STD_WINDOW_FLAG_taskbar;

	// any window created from this point on will not be able to cover taskbar window, except cursor or course :D

	// window content ready for display
	gui_window_taskbar -> flags |= STD_WINDOW_FLAG_fixed_xy | STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	//----------------------------------------------------------------------

	// properties of assigned image to cursor window
	struct LIB_IMAGE_TGA_STRUCTURE *image_cursor = (struct LIB_IMAGE_TGA_STRUCTURE *) &file_cursor_start;

	// cursor window properties
	wm_request -> type = STD_IPC_TYPE_internal;
	wm_request -> request_or_answer = STD_WINDOW_REQUEST_create;
	wm_request -> x = gui_wallpaper_width >> STD_SHIFT_2;
	wm_request -> y = gui_wallpaper_height >> STD_SHIFT_2;
	wm_request -> width = image_cursor -> width;
	wm_request -> height = image_cursor -> height;

	// send request to Window Manager
	std_ipc_send( wm_pid, (uint8_t *) wm_request );

	// wait for answer
	while( ! std_ipc_receive( (uint8_t *) wm_data ) );

	// window assigned?
	wm_answer = (struct STD_IPC_STRUCTURE_WINDOW_ANSWER_DESCRIPTOR *) &wm_data;
	if( ! wm_answer -> descriptor ) return FALSE;	// no

	// properties of cursor window
	gui_window_cursor = (struct STD_WINDOW_STRUCTURE_DESCRIPTOR *) wm_answer -> descriptor;

	// copy image content to cursor window
	uint32_t *pixel_cursor = (uint32_t *) ((uintptr_t) gui_window_cursor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ));
	lib_image_tga_parse( (uint8_t *) image_cursor, pixel_cursor, (uint64_t) file_cursor_end - (uint64_t) file_cursor_start );

	// mark window as cursor, so Window Manager will treat it different than others
	gui_window_cursor -> flags |= STD_WINDOW_FLAG_cursor;

	// yep, any window can be a cursor :) but only 1 with highest internal ID will be treated as it
	// so before you assign a flag to another window, make sure no other window have it

	// window content ready for display
	gui_window_cursor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	// everything prepared
	return TRUE;
}