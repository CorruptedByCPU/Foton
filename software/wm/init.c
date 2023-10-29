/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_init( void ) {
	// get our PID number
	wm_pid = std_pid();

	// obtain information about kernel framebuffer
	std_framebuffer( &kernel_framebuffer );

	// framebuffer locked?
	if( kernel_framebuffer.pid != wm_pid ) { log( "WM: Framebuffer is already in use by process with ID %u.", wm_pid ); while( TRUE ); }

	//----------------------------------------------------------------------

	// prepare space for an array of objects
	wm_object_base_address = (struct WM_STRUCTURE_OBJECT *) malloc( TRUE );

	// prepare space for an array of object list
	wm_list_base_address = (struct WM_STRUCTURE_OBJECT **) malloc( TRUE );

	// prepare space for an list of zones
	wm_zone_base_address = (struct WM_STRUCTURE_ZONE *) malloc( TRUE );

	//----------------------------------------------------------------------

	// create cache space

	// as local object
	wm_object_cache.width		= kernel_framebuffer.width_pixel;
	wm_object_cache.height		= kernel_framebuffer.height_pixel;
	wm_object_cache.descriptor	= (struct STD_WINDOW_STRUCTURE_DESCRIPTOR *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( (wm_object_cache.width * wm_object_cache.height * STD_VIDEO_DEPTH_byte) + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ) ) >> STD_SHIFT_PAGE );

	// leave cache untouched, first objects synchronization will fill it up

	//----------------------------------------------------------------------

	// properties of assigned image to workbench object
	struct LIB_IMAGE_TGA_STRUCTURE *workbench_image = (struct LIB_IMAGE_TGA_STRUCTURE *) &file_wallpaper_start;

	// create workbench object
	wm_object_workbench = wm_object_create( 0, 0, wm_object_cache.width, wm_object_cache.height );

	// convert image to RGBA
	uint32_t *tmp_workbench_image = (uint32_t *) malloc( wm_object_workbench -> width * wm_object_workbench -> height * STD_VIDEO_DEPTH_byte );
	lib_image_tga_parse( (uint8_t *) workbench_image, tmp_workbench_image, (uint64_t) file_wallpaper_end - (uint64_t) file_wallpaper_start );

	// copy scaled image content to workbench object
	uint32_t *workbench_pixel = (uint32_t *) ((uintptr_t) wm_object_workbench -> descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR ));
	float x_scale_factor = (float) ((float) workbench_image -> width / (float) wm_object_workbench -> width);
	float y_scale_factor = (float) ((float) workbench_image -> height / (float) wm_object_workbench -> height);
	for( uint16_t y = 0; y < wm_object_workbench -> height; y++ )
		for( uint16_t x = 0; x < wm_object_workbench -> width; x++ )
			workbench_pixel[ (y * wm_object_workbench -> width) + x ] = tmp_workbench_image[ (uint64_t) (((uint64_t) (y_scale_factor * y) * workbench_image -> width) + (uint64_t) (x * x_scale_factor)) ];

	// release temporary image
	free( tmp_workbench_image );

	// object content ready for display
	wm_object_workbench -> descriptor -> flags |= STD_WINDOW_FLAG_fixed_z | STD_WINDOW_FLAG_fixed_xy | STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

	//----------------------------------------------------------------------

	// properties of assigned image to cursor object
	struct LIB_IMAGE_TGA_STRUCTURE *cursor_image = (struct LIB_IMAGE_TGA_STRUCTURE *) &file_cursor_start;

	// create cursor object
	wm_object_cursor = wm_object_create( wm_object_workbench -> width >> STD_SHIFT_2, wm_object_workbench -> height >> STD_SHIFT_2, cursor_image -> width, cursor_image -> height );

	// copy image content to cursor object
	lib_image_tga_parse( (uint8_t *) cursor_image, (uint32_t *) ((uintptr_t) wm_object_cursor -> descriptor + sizeof( struct STD_WINDOW_STRUCTURE_DESCRIPTOR )), (uint64_t) file_cursor_end - (uint64_t) file_cursor_start );

	// mark window as cursor, so Window Manager will treat it different than others
	wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_cursor;

	// yep, any object can be a cursor :) but only 1 with highest internal ID will be treated as it
	// so before you assign a flag to another object, make sure no other object have it

	// object content ready for display
	wm_object_cursor -> descriptor -> flags |= STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;
}