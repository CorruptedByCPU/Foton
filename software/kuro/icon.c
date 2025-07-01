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
				if( ! kuro_icon[ PLAIN_TEXT ] ) kuro_icon_register( PLAIN_TEXT, (uint8_t *) "/var/share/media/icon/default/mimetypes/text-plain.tga" );

				// assign
				return PLAIN_TEXT;
			}

			// check for log file
			if( file -> name_limit > 4 && lib_string_compare( (uint8_t *) &file -> name[ file -> name_limit - 4 ], (uint8_t *) ".log", 4 ) ) {
				// load text icon (if not present)
				if( ! kuro_icon[ LOG ] ) kuro_icon_register( LOG, (uint8_t *) "/var/share/media/icon/default/mimetypes/text-x-log.tga" );

				// assign
				return LOG;
			}

			// check for C header file
			if( file -> name_limit > 2 && lib_string_compare( (uint8_t *) &file -> name[ file -> name_limit - 2 ], (uint8_t *) ".h", 2 ) ) {
				// load C header icon (if not present)
				if( ! kuro_icon[ HEADER ] ) kuro_icon_register( HEADER, (uint8_t *) "/var/share/media/icon/default/mimetypes/text-x-chdr.tga" );

				// assign
				return HEADER;
			}

			// check for image file
			if( file -> name_limit > 4 && lib_string_compare( (uint8_t *) &file -> name[ file -> name_limit - 4 ], (uint8_t *) ".tga", 4 ) ) {
				// load image icon (if not present)
				if( ! kuro_icon[ IMAGE ] ) kuro_icon_register( IMAGE, (uint8_t *) "/var/share/media/icon/default/mimetypes/image-icon.tga" );

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
						if( ! kuro_icon[ LIBRARY ] ) kuro_icon_register( LIBRARY, (uint8_t *) "/var/share/media/icon/default/mimetypes/application-x-sharedlib.tga" );

						// release file content
						free( elf );

						// assign
						return LIBRARY;
					} else

					// module?
					if( file -> name_limit > 3 && lib_string_compare( (uint8_t *) &file -> name[ file -> name_limit - 3 ], (uint8_t *) ".ko", 3 ) ) {
						// load module icon (if not present)
						if( ! kuro_icon[ MODULE ] ) kuro_icon_register( MODULE, (uint8_t *) "/var/share/media/icon/default/mimetypes/text-x-hex.tga" );

						// release file content
						free( elf );

						// assign
						return MODULE;
					}

					else {
						// load executable icon (if not present)
						if( ! kuro_icon[ EXECUTABLE ] ) kuro_icon_register( EXECUTABLE, (uint8_t *) "/var/share/media/icon/default/app/application-x-executable.tga" );

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

void kuro_icon_register( uint8_t type, uint8_t *path ) {
	// register new icon
	kuro_icon = (uint32_t **) realloc( kuro_icon, type * sizeof( uint32_t * ) );
	kuro_icon[ type ] = lib_image_scale( lib_ui_icon( path ), 48, 48, 16, 16 );
}
