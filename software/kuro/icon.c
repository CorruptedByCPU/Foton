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

			// check for object file
			if( file -> name_limit > 4 && lib_string_compare( (uint8_t *) &file -> name[ file -> name_limit - 4 ], (uint8_t *) ".obj", 4 ) ) {
				// load object icon (if not present)
				if( ! kuro_icon[ OBJECT ] ) kuro_icon_register( OBJECT, (uint8_t *) "/var/share/media/icon/default/object-group.tga" );

				// assign
				return OBJECT;
			}

			// check for image file
			if( file -> name_limit > 4 && lib_string_compare( (uint8_t *) &file -> name[ file -> name_limit - 4 ], (uint8_t *) ".tga", 4 ) ) {
				// load image icon (if not present)
				if( ! kuro_icon[ IMAGE ] ) kuro_icon_register( IMAGE, (uint8_t *) "/var/share/media/icon/default/image-icon.tga" );

				// assign
				return IMAGE;
			}

			// 
			if( file -> limit > sizeof( struct LIB_ELF_STRUCTURE ) ) {
				// properties of file
				struct LIB_ELF_STRUCTURE *elf = (struct LIB_ELF_STRUCTURE *) malloc( sizeof( struct LIB_ELF_STRUCTURE ) );

				// retrieve part of file content
				FILE *f = fopen( file -> name, EMPTY ); fread( f, (uint8_t *) elf, sizeof( struct LIB_ELF_STRUCTURE ) ); fclose( f );

				// ELF file?
				if( lib_elf_identify( (uintptr_t) elf ) ) {
					// library?
					if( elf -> type == LIB_ELF_TYPE_shared_object ) {
						// load library icon (if not present)
						if( ! kuro_icon[ LIBRARY ] ) kuro_icon_register( LIBRARY, (uint8_t *) "/var/share/media/icon/default/application-x-sharedlib.tga" );

						// release file content
						free( elf );

						// assign
						return LIBRARY;
					} else

					// module?
					if( file -> name_limit > 3 && lib_string_compare( (uint8_t *) &file -> name[ file -> name_limit - 3 ], (uint8_t *) ".ko", 3 ) ) {
						// load module icon (if not present)
						if( ! kuro_icon[ MODULE ] ) kuro_icon_register( MODULE, (uint8_t *) "/var/share/media/icon/default/text-x-hex.tga" );

						// release file content
						free( elf );

						// assign
						return MODULE;
					}

					else {
						// load executable icon (if not present)
						if( ! kuro_icon[ EXECUTABLE ] ) kuro_icon_register( EXECUTABLE, (uint8_t *) "/var/share/media/icon/default/application-x-executable.tga" );

						// release file content
						free( elf );

						// assign
						return EXECUTABLE;
					}
				}

				// release file content
				free( elf );
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
