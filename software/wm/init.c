/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t wm_init( void ) {
	// get our PID number
	wm_pid = std_pid();

	// obtain information about kernel framebuffer
	std_framebuffer( (struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER *) &kernel_framebuffer );

	// framebuffer locked?
	if( kernel_framebuffer.pid != wm_pid ) { printf( "WM: Framebuffer is already in use by process with ID %u.", wm_pid ); return FALSE; }

	//----------------------------------------------------------------------

	// prepare area for an array of objects
	wm_object_base_address = (struct WM_STRUCTURE_OBJECT *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct WM_STRUCTURE_OBJECT ) * WM_OBJECT_LIMIT ) >> STD_SHIFT_PAGE );

	// prepare area for a list of objects
	wm_list_base_address = (struct WM_STRUCTURE_OBJECT **) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct WM_STRUCTURE_OBJECT * ) * (WM_LIST_LIMIT + 1) ) >> STD_SHIFT_PAGE );	// blank entry at the end of object list

	// prepare area for a list of zones
	wm_zone_base_address = (struct WM_STRUCTURE_ZONE *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct WM_STRUCTURE_ZONE ) * WM_ZONE_LIMIT ) >> STD_SHIFT_PAGE );

	// prepare area for a taskbar list
	wm_taskbar_base_address = (struct WM_STRUCTURE_OBJECT **) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct WM_STRUCTURE_OBJECT * ) * WM_TASKBAR_LIMIT ) >> STD_SHIFT_PAGE );

	//----------------------------------------------------------------------

	// create cache area

	// as local object
	wm_object_cache.width		= kernel_framebuffer.width_pixel;
	wm_object_cache.height		= kernel_framebuffer.height_pixel;
	wm_object_cache.descriptor	= (struct STD_STRUCTURE_WINDOW_DESCRIPTOR *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( (wm_object_cache.width * wm_object_cache.height * STD_VIDEO_DEPTH_byte) + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ) ) >> STD_SHIFT_PAGE );

	// leave cache untouched, first objects synchronization will fill it up

	//----------------------------------------------------------------------

	// properties of file
	struct STD_STRUCTURE_FILE workbench_file = { EMPTY };

	// properties of image
	struct LIB_IMAGE_STRUCTURE_TGA *workbench_image = EMPTY;

	// retrieve file information
	uint8_t wallpaper_path[] = "/system/var/gfx/wallpapers/default.tga";
	if( (workbench_file.socket = std_file_open( (uint8_t *) &wallpaper_path, sizeof( wallpaper_path ) - 1 )) ) {
		// retrieve file properties
		std_file( (struct STD_STRUCTURE_FILE *) &workbench_file );

		// assign area for file
		workbench_image = (struct LIB_IMAGE_STRUCTURE_TGA *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( workbench_file.byte ) >> STD_SHIFT_PAGE );

		// load file content
		std_file_read( (struct STD_STRUCTURE_FILE *) &workbench_file, (uint8_t *) workbench_image, workbench_file.byte );
	}

	// create workbench object
	wm_object_workbench = wm_object_create( 0, 0, wm_object_cache.width, wm_object_cache.height );

	// mark it as our
	wm_object_workbench -> pid = wm_pid;

	// properties of workbench area content
	uint32_t *workbench_pixel = (uint32_t *) ((uintptr_t) wm_object_workbench -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ));

	// if default wallpaper file found
	if( workbench_image ) {
		// convert image to RGBA
		uint32_t *tmp_workbench_image = (uint32_t *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( wm_object_workbench -> size_byte ) >> STD_SHIFT_PAGE );
		lib_image_tga_parse( (uint8_t *) workbench_image, tmp_workbench_image, workbench_file.byte );

		// copy scaled image content to workbench object
		float x_scale_factor = (float) ((float) workbench_image -> width / (float) wm_object_workbench -> width);
		float y_scale_factor = (float) ((float) workbench_image -> height / (float) wm_object_workbench -> height);
		for( uint16_t y = 0; y < wm_object_workbench -> height; y++ )
			for( uint16_t x = 0; x < wm_object_workbench -> width; x++ )
				workbench_pixel[ (y * wm_object_workbench -> width) + x ] = tmp_workbench_image[ (uint64_t) (((uint64_t) (y_scale_factor * y) * workbench_image -> width) + (uint64_t) (x * x_scale_factor)) ];

		// release temporary image
		std_memory_release( (uintptr_t) tmp_workbench_image, MACRO_PAGE_ALIGN_UP( wm_object_workbench -> size_byte ) >> STD_SHIFT_PAGE );

		// release file content
		std_memory_release( (uintptr_t) workbench_image, MACRO_PAGE_ALIGN_UP( workbench_file.byte ) >> STD_SHIFT_PAGE );

		// close file
		std_file_close( workbench_file.socket );
	} else
		// fill workbench with default color
		for( uint16_t y = 0; y < wm_object_workbench -> height; y++ )
			for( uint16_t x = 0; x < wm_object_workbench -> width; x++ )
				workbench_pixel[ (y * wm_object_workbench -> width) + x ] = 0xFF101010;

	// show debug information
	// uint8_t build_version[] = "Foton v"KERNEL_version"."KERNEL_revision" build on "__DATE__" "__TIME__;
	// lib_font( LIB_FONT_FAMILY_ROBOTO_MONO, build_version, sizeof( build_version ) - 1, STD_COLOR_GRAY, workbench_pixel, wm_object_workbench -> width, LIB_FONT_ALIGN_right );

	// object content ready for display
	wm_object_workbench -> descriptor -> flags |= STD_WINDOW_FLAG_fixed_z | STD_WINDOW_FLAG_fixed_xy | STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	//----------------------------------------------------------------------

	// execute workbench function as thread
	uint8_t wm_string_workbench[] = "wm workbench";
	std_thread( (uintptr_t) &wm_workbench, (uint8_t *) &wm_string_workbench, sizeof( wm_string_workbench ) );

	//----------------------------------------------------------------------

	// create taskbar object
	wm_object_taskbar = wm_object_create( 0, wm_object_workbench -> height - WM_OBJECT_TASKBAR_HEIGHT_pixel, wm_object_workbench -> width, WM_OBJECT_TASKBAR_HEIGHT_pixel );

	// mark object as taskbar and unmovable
	wm_object_taskbar -> descriptor -> flags = STD_WINDOW_FLAG_taskbar | STD_WINDOW_FLAG_fixed_z | STD_WINDOW_FLAG_fixed_xy;

	// fill taskbar with default background color
	uint32_t *taskbar_pixel = (uint32_t *) ((uintptr_t) wm_object_taskbar -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ));
	for( uint16_t y = 0; y < wm_object_taskbar -> height; y++ )
		for( uint16_t x = 0; x < wm_object_taskbar -> width; x++ )
			taskbar_pixel[ (y * wm_object_taskbar -> width) + x ] = WM_TASKBAR_BG_default;

	// show menu buton on taskbar
	uint8_t test[ 3 ] = "|||";
	lib_font( LIB_FONT_FAMILY_ROBOTO, (uint8_t *) &test, sizeof( test ), 0xFFFFFFFF, taskbar_pixel + (((WM_OBJECT_TASKBAR_HEIGHT_pixel - LIB_FONT_HEIGHT_pixel) / 2) * wm_object_taskbar -> width) + (22 >> STD_SHIFT_2), wm_object_taskbar -> width, LIB_FONT_ALIGN_center );

	// object content ready for display
	wm_object_taskbar -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	// execute taskbar function as thread
	uint8_t wm_string_taskbar[] = "wm taskbar";
	wm_object_taskbar -> pid = std_thread( (uintptr_t) &wm_taskbar, (uint8_t *) &wm_string_taskbar, sizeof( wm_string_taskbar ) );

	//----------------------------------------------------------------------

	// execute menu function as thread
	uint8_t wm_string_menu[] = "wm menu";
	std_thread( (uintptr_t) &wm_menu, (uint8_t *) &wm_string_menu, sizeof( wm_string_menu ) );

	//----------------------------------------------------------------------

	// execute clock function as thread
	uint8_t wm_string_clock[] = "wm clock";
	std_thread( (uintptr_t) &wm_clock, (uint8_t *) &wm_string_clock, sizeof( wm_string_clock ) );

	//----------------------------------------------------------------------

	// create lock object
	wm_object_lock = wm_object_create( 0, 0, wm_object_cache.width, wm_object_cache.height );

	// mark it as our
	wm_object_lock -> pid = wm_pid;

	// object content ready for display
	wm_object_lock -> descriptor -> flags |= STD_WINDOW_FLAG_lock | STD_WINDOW_FLAG_fixed_xy;

	// move object above taskbar
	wm_object_move_up( wm_object_lock );

	// properties of workbench area content
	uint32_t *lock_pixel = (uint32_t *) ((uintptr_t) wm_object_lock -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ));

	// fill lock with default shadow
	for( uint16_t y = 0; y < wm_object_lock -> height; y++ )
		for( uint16_t x = 0; x < wm_object_lock -> width; x++ )
			lock_pixel[ (y * wm_object_lock -> width) + x ] = WM_LOCK_BACKGROUND_color;

	//----------------------------------------------------------------------

	// properties of file
	struct STD_STRUCTURE_FILE cursor_file = { EMPTY };

	// properties of image
	struct LIB_IMAGE_STRUCTURE_TGA *cursor_image = EMPTY;

	// retrieve file information
	uint8_t cursor_path[] = "/system/var/gfx/cursors/default.tga";
	if( (cursor_file.socket = std_file_open( (uint8_t *) &cursor_path, sizeof( cursor_path ) - 1 )) ) {
		// retrieve properties of file
		std_file( (struct STD_STRUCTURE_FILE *) &cursor_file );

		// assign area for file
		cursor_image = (struct LIB_IMAGE_STRUCTURE_TGA *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( cursor_file.byte ) >> STD_SHIFT_PAGE );

		// load file content
		if( cursor_image ) std_file_read( (struct STD_STRUCTURE_FILE *) &cursor_file, (uint8_t *) cursor_image, cursor_file.byte );

		// create cursor object
		wm_object_cursor = wm_object_create( wm_object_workbench -> width >> STD_SHIFT_2, wm_object_workbench -> height >> STD_SHIFT_2, cursor_image -> width, cursor_image -> height );
	} else
		// create default object
		wm_object_cursor = wm_object_create( wm_object_workbench -> width >> STD_SHIFT_2, wm_object_workbench -> height >> STD_SHIFT_2, 16, 32 );

	// mark it as our
	wm_object_cursor -> pid = wm_pid;

	// properties of cursor area content
	uint32_t *cursor_pixel = (uint32_t *) ((uintptr_t) wm_object_cursor -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ));

	// fill cursor with default color
	for( uint16_t y = 0; y < wm_object_cursor -> height; y++ )
		for( uint16_t x = 0; x < wm_object_cursor -> width; x++ )
			cursor_pixel[ (y * wm_object_cursor -> width) + x ] = STD_COLOR_WHITE;

	// if default cursor file found
	if( cursor_image ) {
		// copy image content to cursor object
		lib_image_tga_parse( (uint8_t *) cursor_image, cursor_pixel, cursor_file.byte );

		// release file content
		std_memory_release( (uintptr_t) cursor_image, MACRO_PAGE_ALIGN_UP( cursor_file.byte ) >> STD_SHIFT_PAGE );

		// close file
		std_file_close( cursor_file.socket );
	}

	// mark window as cursor, so Window Manager will treat it different than others
	wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_cursor;

	// yep, any object can be a cursor :) but only 1 with highest internal ID will be treated as it
	// so before you assign a flag to another object, make sure no other object have it

	// object content ready for display
	wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	//----------------------------------------------------------------------

	// debug
	std_exec( (uint8_t *) "kuro", 4, EMPTY, TRUE );

	// Window Manager initialized.
	return TRUE;
}
