/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t kuro_icon_assign( struct STD_STRUCTURE_DIR *file ) {
	// select icon by file type
	switch( file -> type ) {
		case STD_FILE_TYPE_link:	return UP;
		case STD_FILE_TYPE_directory:	return DIRECTORY;
		default: {
			// check for plain text
			if( file -> name_limit > 4 && lib_string_compare( (uint8_t *) &file -> name[ file -> name_limit - 4 ], (uint8_t *) ".txt", 4 ) ) {
				// load text icon (if not present)
				if( ! kuro_icon[ PLAIN_TEXT ] ) kuro_icon_register( PLAIN_TEXT, (uint8_t *) "/var/share/media/icon/default/text-plain.tga" );

				// assign
				return PLAIN_TEXT;
			}

			// check for log file
			if( file -> name_limit > 4 && lib_string_compare( (uint8_t *) &file -> name[ file -> name_limit - 4 ], (uint8_t *) ".log", 4 ) ) {
				// load text icon (if not present)
				if( ! kuro_icon[ LOG ] ) kuro_icon_register( LOG, (uint8_t *) "/var/share/media/icon/default/text-x-log.tga" );

				// assign
				return LOG;
			}
		}
	}

	// default
	return UNKNOWN;
}

uint32_t *kuro_icon_load( uint8_t *path ) {
	// file properties
	FILE *file = EMPTY;

	// file exist?
	if( (file = fopen( path, EMPTY )) ) {
		// assign area for file
		struct LIB_IMAGE_STRUCTURE_TGA *image = (struct LIB_IMAGE_STRUCTURE_TGA *) malloc( MACRO_PAGE_ALIGN_UP( file -> byte ) >> STD_SHIFT_PAGE );

		// load file content
		fread( file, (uint8_t *) image, file -> byte );

		// copy image content to cursor object
		uint32_t *icon = (uint32_t *) malloc( image -> width * image -> height * STD_VIDEO_DEPTH_byte );
		lib_image_tga_parse( (uint8_t *) image, icon, file -> byte );

		// release file content
		free( image );

		// close file
		fclose( file );

		// done
		return icon;
	}

	// cannot locate specified file
	return EMPTY;
}

void kuro_icon_register( uint8_t type, uint8_t *path ) {
	// register new icon
	kuro_icon = (uint32_t **) realloc( kuro_icon, type * sizeof( uint32_t * ) );
	kuro_icon[ type ] = lib_image_scale( kuro_icon_load( path ), 48, 48, 16, 16 );
}
