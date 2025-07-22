/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#include	"./kuro.h"
	#include	"./elf.h"
	#include	"./image.h"
	#include	"./integer.h"
	#include	"./string.h"
	#include	"./ui.h"
	//----------------------------------------------------------------------

void lib_kuro_icon_register( struct LIB_KURO_STRUCTURE *kuro, uint8_t type, uint8_t *path ) {
	// register new icon
	kuro -> icon = (uint32_t **) realloc( kuro -> icon, type * sizeof( uint32_t * ) );
	kuro -> icon[ type ] = lib_image_scale( lib_icon_icon( path ), 48, 48, 16, 16 );
}

uint8_t lib_kuro_icon_assign( struct LIB_KURO_STRUCTURE *kuro, struct STD_STRUCTURE_DIR *file ) {
	// select icon by file type
	switch( file -> type ) {
		case STD_FILE_TYPE_link:	return LIB_KURO_MIMETYPE_UP;
		case STD_FILE_TYPE_directory:	return LIB_KURO_MIMETYPE_DIRECTORY;
		default: {
			// check for plain text
			if( file -> name_limit > 4 && lib_string_compare( (uint8_t *) &file -> name[ file -> name_limit - 4 ], (uint8_t *) ".txt", 4 ) ) {
				// load text icon (if not present)
				if( ! kuro -> icon[ LIB_KURO_MIMETYPE_PLAIN_TEXT ] ) lib_kuro_icon_register( kuro, LIB_KURO_MIMETYPE_PLAIN_TEXT, (uint8_t *) "/var/share/media/icon/default/mimetypes/text-plain.tga" );

				// assign
				return LIB_KURO_MIMETYPE_PLAIN_TEXT;
			}

			// check for log file
			if( file -> name_limit > 4 && lib_string_compare( (uint8_t *) &file -> name[ file -> name_limit - 4 ], (uint8_t *) ".log", 4 ) ) {
				// load text icon (if not present)
				if( ! kuro -> icon[ LIB_KURO_MIMETYPE_LOG ] ) lib_kuro_icon_register( kuro, LIB_KURO_MIMETYPE_LOG, (uint8_t *) "/var/share/media/icon/default/mimetypes/text-x-log.tga" );

				// assign
				return LIB_KURO_MIMETYPE_LOG;
			}

			// check for C header file
			if( file -> name_limit > 2 && lib_string_compare( (uint8_t *) &file -> name[ file -> name_limit - 2 ], (uint8_t *) ".h", 2 ) ) {
				// load C header icon (if not present)
				if( ! kuro -> icon[ LIB_KURO_MIMETYPE_HEADER ] ) lib_kuro_icon_register( kuro, LIB_KURO_MIMETYPE_HEADER, (uint8_t *) "/var/share/media/icon/default/mimetypes/text-x-chdr.tga" );

				// assign
				return LIB_KURO_MIMETYPE_HEADER;
			}

			// check for image file
			if( file -> name_limit > 4 && lib_string_compare( (uint8_t *) &file -> name[ file -> name_limit - 4 ], (uint8_t *) ".tga", 4 ) ) {
				// load image icon (if not present)
				if( ! kuro -> icon[ LIB_KURO_MIMETYPE_IMAGE ] ) lib_kuro_icon_register( kuro, LIB_KURO_MIMETYPE_IMAGE, (uint8_t *) "/var/share/media/icon/default/mimetypes/image-icon.tga" );

				// assign
				return LIB_KURO_MIMETYPE_IMAGE;
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
						if( ! kuro -> icon[ LIB_KURO_MIMETYPE_LIBRARY ] ) lib_kuro_icon_register( kuro, LIB_KURO_MIMETYPE_LIBRARY, (uint8_t *) "/var/share/media/icon/default/mimetypes/application-x-sharedlib.tga" );

						// release file content
						free( elf );

						// assign
						return LIB_KURO_MIMETYPE_LIBRARY;
					} else

					// module?
					if( file -> name_limit > 3 && lib_string_compare( (uint8_t *) &file -> name[ file -> name_limit - 3 ], (uint8_t *) ".ko", 3 ) ) {
						// load module icon (if not present)
						if( ! kuro -> icon[ LIB_KURO_MIMETYPE_MODULE ] ) lib_kuro_icon_register( kuro, LIB_KURO_MIMETYPE_MODULE, (uint8_t *) "/var/share/media/icon/default/mimetypes/text-x-hex.tga" );

						// release file content
						free( elf );

						// assign
						return LIB_KURO_MIMETYPE_MODULE;
					}

					else {
						// load executable icon (if not present)
						if( ! kuro -> icon[ LIB_KURO_MIMETYPE_EXECUTABLE ] ) lib_kuro_icon_register( kuro, LIB_KURO_MIMETYPE_EXECUTABLE, (uint8_t *) "/var/share/media/icon/default/app/application-x-executable.tga" );

						// release file content
						free( elf );

						// assign
						return LIB_KURO_MIMETYPE_EXECUTABLE;
					}
				}

				// release file content
				free( elf );
			}
		}
	}

	// default
	return LIB_KURO_MIMETYPE_UNKNOWN;
}

void lib_kuro_dir_add( struct LIB_KURO_STRUCTURE *kuro, struct STD_STRUCTURE_DIR *dir ) {
	// insert new entries
	uint64_t file = FALSE;
	while( dir[ ++file ].type ) {
		// extend table content by entry
		kuro -> entries = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ENTRY *) realloc( kuro -> entries, (kuro -> rows + 1) * sizeof( struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ENTRY ) );

		// default flag state
		kuro -> entries[ kuro -> rows ].flag = EMPTY;	// none

		// assign area for entry
		kuro -> entries[ kuro -> rows ].cell = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE_CELL *) calloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_TABLE_CELL ) * kuro -> cols );

		//
		kuro -> entries[ kuro -> rows ].reserved = lib_kuro_icon_assign( kuro, (struct STD_STRUCTURE_DIR *) &dir[ file ] );

		// column 0 --------------------------------------------
		uint64_t col = 0;

		// assign proper icon
		kuro -> entries[ kuro -> rows ].cell[ col ].icon = kuro -> icon[ kuro -> entries[ kuro -> rows ].reserved ];

		// set cell: name
		kuro -> entries[ kuro -> rows ].cell[ col ].name = (uint8_t *) calloc( dir[ file ].name_limit + TRUE );
		for( uint64_t i = 0; i < dir[ file ].name_limit; i++ ) kuro -> entries[ kuro -> rows ].cell[ col ].name[ i ] = dir[ file ].name[ i ];

		if( kuro -> flag & LIB_KURO_FLAG_size ) {
			// column 1 --------------------------------------------
			col++;

			// align content to right
			kuro -> entries[ kuro -> rows ].cell[ col ].flag = LIB_FONT_FLAG_ALIGN_right;

			// set cell: size
			// only if thats not a special link or directory
			if( file != TRUE && dir[ file ].type != STD_FILE_TYPE_directory ) {
				kuro -> entries[ kuro -> rows ].cell[ col ].name = (uint8_t *) calloc( lib_integer_digit_count( dir[ file ].limit, STD_NUMBER_SYSTEM_decimal ) + TRUE );
				lib_integer_to_string( dir[ file ].limit, STD_NUMBER_SYSTEM_decimal, kuro -> entries[ kuro -> rows ].cell[ col ].name );
			}
		}

		//------------------------------------------------------

		// entry created
		kuro -> rows++;
	}
}

uint8_t lib_kuro_dir_compare( struct STD_STRUCTURE_DIR *first, struct STD_STRUCTURE_DIR *second ) {
	// minimal length
	size_t length = first -> name_limit;
	if( second -> name_limit < length ) length = second -> name_limit;

	// check which name is lower in alphabetical order
	for( size_t i = 0; i < length; i++ ) {
		// retrieve characters
		uint8_t first_char = first -> name[ i ]; if( first_char > '`' && first_char < '{' ) first_char -= STD_ASCII_SPACE;
		uint8_t second_char = second -> name[ i ]; if( second_char > '`' && second_char < '{' ) second_char -= STD_ASCII_SPACE;

		// compare characters, return higher
		if( first_char > second_char ) return TRUE;
		else return FALSE;
	}

	// both are equal
	return FALSE;
}

// bubble sort, thats only for pre-alpha release, so why not?
void lib_kuro_dir_sort( struct STD_STRUCTURE_DIR *entries, uint64_t n ) {
	// prepare area for separated directories and files
	struct STD_STRUCTURE_DIR *directories = (struct STD_STRUCTURE_DIR *) malloc( (n + 1) * sizeof( struct STD_STRUCTURE_DIR ) );
	struct STD_STRUCTURE_DIR *files = (struct STD_STRUCTURE_DIR *) malloc( (n + 1) * sizeof( struct STD_STRUCTURE_DIR ) );

	// separate directories from other files
	uint64_t directory = EMPTY;
	uint64_t file = EMPTY;
	for( uint64_t i = 1; i < n; i++ ) {
		// based on type
		switch( entries[ i ].type ) {
			// for directories
			case STD_FILE_TYPE_directory: { directories[ directory++ ] = entries[ i ]; break; }
			// and other files
			default: files[ file++ ] = entries[ i ];
		}
	}

	// reserve infinity for bubble sort
	uint8_t loop = FALSE;

	// sort directories
	while( directory && ! loop ) {
		// until raedy
		loop = TRUE;

		// compare all entries
		for( uint64_t i = 0; i < directory; i++ ) {
			// if first entry name is higher alphabetically than second
			if( lib_kuro_dir_compare( (struct STD_STRUCTURE_DIR *) &directories[ i ], (struct STD_STRUCTURE_DIR *) &directories[ i + 1 ] ) ) {
				// replace them
				struct STD_STRUCTURE_DIR tmp = directories[ i ]; directories[ i ] = directories[ i + 1 ]; directories[ i + 1 ] = tmp;

				// and check all entries again
				loop = FALSE;
			}
		}
	}

	// sort other files
	loop = FALSE; while( file && ! loop ) {
		// until raedy
		loop = TRUE;

		// compare all entries
		for( uint64_t i = 0; i < file; i++ ) {
			// if first entry name is higher alphabetically than second
			if( lib_kuro_dir_compare( (struct STD_STRUCTURE_DIR *) &files[ i ], (struct STD_STRUCTURE_DIR *) &files[ i + 1 ] ) ) {
				// replace them
				struct STD_STRUCTURE_DIR tmp = files[ i ]; files[ i ] = files[ i + 1 ]; files[ i + 1 ] = tmp;

				// and check all entries again
				loop = FALSE;
			}
		}
	}

	// combine results
	for( uint64_t i = 0; i < directory; i++ ) entries[ i + 1 ] = directories[ i ];
	for( uint64_t i = 0; i < file; i++ ) entries[ i + 1 + directory ] = files[ i ];
}

void lib_kuro( struct LIB_KURO_STRUCTURE *kuro ) {
	// if not exist
	if( ! kuro -> icon ) {
		// initialize icon list
		kuro -> icon = (uint32_t **) malloc( TRUE );

		// register initial icon (directory change)
		lib_kuro_icon_register( kuro, LIB_KURO_MIMETYPE_UP, (uint8_t *) "/var/share/media/icon/default/empty.tga" );
		lib_kuro_icon_register( kuro, LIB_KURO_MIMETYPE_DIRECTORY, (uint8_t *) "/var/share/media/icon/default/places/folder.tga" );
		lib_kuro_icon_register( kuro, LIB_KURO_MIMETYPE_UNKNOWN, (uint8_t *) "/var/share/media/icon/default/mimetypes/unknown.tga" );
	}

	// retrieve list of files inside current directory
	struct STD_STRUCTURE_DIR *dir = (struct STD_STRUCTURE_DIR *) std_dir( (uint8_t *) ".", TRUE );

	// count amount of entries
	uint64_t file_count = EMPTY;
	while( dir[ ++file_count ].type );

	// arrange files in alphabetical order starting from directories
	lib_kuro_dir_sort( (struct STD_STRUCTURE_DIR *) &dir[ TRUE ], file_count - TRUE );	// ignore first entry, not usable for us

	// by default
	kuro -> cols = TRUE;	// only 1 column

	// enable other?
	if( kuro -> flag & LIB_KURO_FLAG_size ) kuro -> cols++;	// yes

	// there is no header?
	if( ! kuro -> header ) {	// && kuro -> flag & LIB_KURO_FLAG_header ) {
		// create one
		kuro -> header = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE_HEADER *) calloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_TABLE_HEADER ) * kuro -> cols );

		// column 0
		uint8_t column_name[] = "Name:";
		kuro -> header[ FALSE ].cell.name = (uint8_t *) malloc( sizeof( column_name ) );
		for( uint8_t i = 0; i < sizeof( column_name ); i++ ) kuro -> header[ FALSE ].cell.name[ i ] = column_name[ i ];

		kuro -> header[ FALSE ].cell.flag = LIB_FONT_FLAG_WEIGHT_bold;

		if( kuro -> flag & LIB_KURO_FLAG_size ) {
			// column Last
			uint8_t column_size[] = "Size:";
			kuro -> header[ TRUE ].cell.name = (uint8_t *) malloc( sizeof( column_size ) );
			for( uint8_t i = 0; i < sizeof( column_size ); i++ ) kuro -> header[ TRUE ].cell.name[ i ] = column_size[ i ];
		
			kuro -> header[ TRUE ].cell.flag = LIB_FONT_FLAG_WEIGHT_bold | LIB_FONT_FLAG_ALIGN_right;
		}
	}

	// there is no table content?
	if( ! kuro -> entries ) {
		// create one
		kuro -> entries = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ENTRY *) malloc( FALSE );

		// insert new entries
		lib_kuro_dir_add( kuro, dir );
	} else {
		// release old table content
		for( uint64_t y = 0; y < kuro -> rows; y++ ) {
			for( uint64_t x = 0; x < kuro -> cols; x++ )
				// name
				if( kuro -> entries[ y ].cell[ x ].name ) free( kuro -> entries[ y ].cell[ x ].name );

			// and cell itself
			free( kuro -> entries[ y ].cell );
		}

		// no entries by default
		kuro -> rows = EMPTY;

		// truncate table content
		kuro -> entries = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ENTRY *) realloc( kuro -> entries, FALSE );

		// insert new entries
		lib_kuro_dir_add( kuro, dir );
	}
}
