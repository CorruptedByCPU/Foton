/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kuro_icon_set( struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY *entry, struct STD_STRUCTURE_DIR *file ) {
	// select icon by file type
	switch( file -> type ) {
		case STD_FILE_TYPE_link: {
			// link
			entry -> mimetype = KURO_MIMETYPE_up;
			entry -> icon = kuro_icons[ entry -> mimetype ];
			
			// done
			break;
		}

		case STD_FILE_TYPE_directory: {
			// load directory icon (if not present)
			if( ! kuro_icons[ KURO_MIMETYPE_directory ] ) kuro_icons_register( KURO_MIMETYPE_directory, (uint8_t *) "/var/share/media/icon/default/folder.tga" );

			// directory
			entry -> mimetype = KURO_MIMETYPE_directory;
			entry -> icon = kuro_icons[ entry -> mimetype ];
			
			// done
			break;
		}

		// other
		default: {
			// load default icon (if not present)
			if( ! kuro_icons[ KURO_MIMETYPE_unknown ] ) kuro_icons_register( KURO_MIMETYPE_unknown, (uint8_t *) "/var/share/media/icon/default/unknown.tga" );

			// default
			entry -> mimetype = KURO_MIMETYPE_unknown;
			entry -> icon = kuro_icons[ entry -> mimetype ];

			// try to check known extensions

			// image?
			if( entry -> name_length > KURO_FILE_EXTENSION_LENGTH_4 && lib_string_compare( (uint8_t *) &entry -> name[ entry -> name_length - KURO_FILE_EXTENSION_LENGTH_4 ], (uint8_t *) ".tga", KURO_FILE_EXTENSION_LENGTH_4 ) ) {
				// load image icon (if not present)
				if( ! kuro_icons[ KURO_MIMETYPE_image ] ) kuro_icons_register( KURO_MIMETYPE_image, (uint8_t *) "/var/share/media/icon/default/image-icon.tga" );

				// set image icon
				entry -> mimetype = KURO_MIMETYPE_image;
				entry -> icon = kuro_icons[ entry -> mimetype ];
			}

			// plain text?
			if( entry -> name_length > KURO_FILE_EXTENSION_LENGTH_4 && lib_string_compare( (uint8_t *) &entry -> name[ entry -> name_length - KURO_FILE_EXTENSION_LENGTH_4 ], (uint8_t *) ".txt", 4 ) ) {
				// load text icon (if not present)
				if( ! kuro_icons[ KURO_MIMETYPE_plain_text ] ) kuro_icons_register( KURO_MIMETYPE_plain_text, (uint8_t *) "/var/share/media/icon/default/text-plain.tga" );

				// set image icon
				entry -> mimetype = KURO_MIMETYPE_plain_text;
				entry -> icon = kuro_icons[ entry -> mimetype ];
			}

			// log?
			if( entry -> name_length > KURO_FILE_EXTENSION_LENGTH_4 && lib_string_compare( (uint8_t *) &entry -> name[ entry -> name_length - KURO_FILE_EXTENSION_LENGTH_4 ], (uint8_t *) ".log", 4 ) ) {
				// load text icon (if not present)
				if( ! kuro_icons[ KURO_MIMETYPE_log ] ) kuro_icons_register( KURO_MIMETYPE_log, (uint8_t *) "/var/share/media/icon/default/text-x-log.tga" );

				// set image icon
				entry -> mimetype = KURO_MIMETYPE_log;
				entry -> icon = kuro_icons[ entry -> mimetype ];
			}

			// 3D object?
			if( entry -> name_length > KURO_FILE_EXTENSION_LENGTH_4 && lib_string_compare( (uint8_t *) &entry -> name[ entry -> name_length - KURO_FILE_EXTENSION_LENGTH_4 ], (uint8_t *) ".obj", KURO_FILE_EXTENSION_LENGTH_4 ) ) {
				// load text icon (if not present)
				if( ! kuro_icons[ KURO_MIMETYPE_3d_object ] ) kuro_icons_register( KURO_MIMETYPE_3d_object, (uint8_t *) "/var/share/media/icon/default/object-group.tga" );

				// set image icon
				entry -> mimetype = KURO_MIMETYPE_3d_object;
				entry -> icon = kuro_icons[ entry -> mimetype ];
			}

			// try to recognize file type by its content, slow...

			// file is empty?
			if( ! entry -> byte ) return;	// yes, nothing to do

// tbd
	// 			// properties of file
	// 			struct LIB_ELF_STRUCTURE *elf = (struct LIB_ELF_STRUCTURE *) malloc( sizeof( struct LIB_ELF_STRUCTURE ) );

	// 			// retrieve part of file content
	// 			FILE *file = fopen( entry -> name, EMPTY ); fread( file, (uint8_t *) elf, sizeof( struct LIB_ELF_STRUCTURE ) ); fclose( file );

	// 			// file type of ELF
	// 			if( lib_elf_identify( (uintptr_t) elf ) ) {	// yes
	// 				// library?
	// 				if( elf -> type == LIB_ELF_TYPE_shared_object ) {
	// 					// load library icon (if not present)
	// 					if( ! kuro_icons[ KURO_MIMETYPE_library ] ) kuro_icons_register( KURO_MIMETYPE_library, (uint8_t *) "system:/var/gfx/icons/application-x-sharedlib.tga" );

	// 					// set library icon
	// 					entry -> mimetype = KURO_MIMETYPE_library;
	// 					entry -> icon = kuro_icons[ entry -> mimetype ];
	// 				}
					
	// 				// module?
	// 				else if( entry -> name_length > 3 && lib_string_compare( (uint8_t *) &entry -> name[ entry -> name_length - 3 ], (uint8_t *) ".ko", 3 ) ) {
	// 					// load module icon (if not present)
	// 					if( ! kuro_icons[ KURO_MIMETYPE_module ] ) kuro_icons_register( KURO_MIMETYPE_module, (uint8_t *) "system:/var/gfx/icons/application-octet-stream.tga" );

	// 					// set module icon
	// 					entry -> mimetype = KURO_MIMETYPE_module;
	// 					entry -> icon = kuro_icons[ entry -> mimetype ];
	// 				}
					
	// 				// executable
	// 				else {
	// 					// load module icon (if not present)
	// 					if( ! kuro_icons[ KURO_MIMETYPE_executable ] ) kuro_icons_register( KURO_MIMETYPE_executable, (uint8_t *) "system:/var/gfx/icons/application-x-executable.tga" );

	// 					// set executable icon
	// 					entry -> mimetype = KURO_MIMETYPE_executable;
	// 					entry -> icon = kuro_icons[ entry -> mimetype ];
	// 				}
	// 			}

	// 			// release file properties
	// 			free( elf );
		}
	}
}
