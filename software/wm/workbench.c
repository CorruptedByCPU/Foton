/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

int64_t wm_workbench( void ) {
	// change current directory
	std_cd( (uint8_t *) "/home/root/workbench", 20 );

	// open workbench directory
	FILE *directory = fopen( (uint8_t *) "." );

	// load its content
	struct LIB_VFS_STRUCTURE *vfs = (struct LIB_VFS_STRUCTURE *) malloc( directory -> byte );
	fread( directory, (uint8_t *) vfs, directory -> byte );

	// initial padding for each icon 
	uint16_t x = 8;
	uint16_t y = 8;

	// for each icon inside workbench directory
	while( vfs -> name_length ) {
		// except defailt symlinks
		if( vfs -> name[ 0 ] == '.' ) { vfs++; continue; }

		// open icon properties and load its content
		FILE *icon_file = fopen( vfs -> name );
		uint8_t *icon_content = (uint8_t *) malloc( icon_file -> byte );
		fread( icon_file, icon_content, icon_file -> byte );
	
		// open image described inside icon properties
		FILE *icon_path = fopen( icon_content );
		struct LIB_IMAGE_STRUCTURE_TGA *icon_image = (struct LIB_IMAGE_STRUCTURE_TGA *) malloc( icon_path -> byte );
		fread( icon_path, (uint8_t *) icon_image, icon_path -> byte );

		// move pointer to icon "name"
		icon_content += lib_string_length( icon_content ) + 1;

		// calculate icon with in pixels
		uint8_t name_pixel = lib_font_length_string( LIB_FONT_FAMILY_ROBOTO, icon_content, lib_string_length( icon_content ) );
		if( name_pixel < 48 ) name_pixel = 48;

		// create icon
		struct WM_STRUCTURE_OBJECT *icon = wm_object_create( x, y, name_pixel, 48 + LIB_FONT_HEIGHT_pixel );
		icon -> pid = wm_pid;	// set owner as "Window Manager"

		// properties of icon area
		uint32_t *icon_pixel = (uint32_t *) ((uintptr_t) icon -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ));

		// convert image to RGBA
		uint32_t *tmp_icon_image = (uint32_t *) malloc( MACRO_PAGE_ALIGN_UP( icon -> size_byte ) );
		lib_image_tga_parse( (uint8_t *) icon_image, tmp_icon_image, icon_path -> byte );

		// copy image content to icon area
		for( uint16_t y = 0; y < icon -> height; y++ )
			for( uint16_t x = 0; x < icon_image -> width; x++ )
				icon_pixel[ (y * name_pixel) + x + ((name_pixel - 48) >> STD_SHIFT_2) ] = tmp_icon_image[ (y * icon_image -> width) + x ];

		// release no more needed areas
		free( icon_image );
		free( tmp_icon_image );

		// release no more needed files
		fclose( icon_path );
		fclose( icon_file );

		// set icon name
		lib_font( LIB_FONT_FAMILY_ROBOTO, icon_content, lib_string_length( icon_content ), LIB_INTERFACE_COLOR_foreground, icon_pixel + (name_pixel >> STD_SHIFT_2) + (48 * name_pixel), name_pixel, LIB_FONT_ALIGN_center );

		// move pointer to command for icon
		icon_content += lib_string_length( icon_content ) + 1;

		// set event for icon
		icon -> descriptor -> name_length = lib_string_length( icon_content );
		for( uint8_t i = 0; i < icon -> descriptor -> name_length; i++ ) icon -> descriptor -> name[ i ] = icon_content[ i ];

		// release no more needed area
		free( icon_content );
	
		// icon ready, show it
		icon -> descriptor -> flags |= STD_WINDOW_FLAG_icon | STD_WINDOW_FLAG_fixed_z | STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;

		// next icon position
		x += name_pixel + 8;

		// next file from workbench directory
		vfs++;
	}

	// release workbench content
	free( vfs );

	// close workbench directory
	fclose( directory );

	// hold the door
	while( TRUE ) sleep( TRUE );

	// exited properly
	return EMPTY;
}
