/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// required libraries
	//----------------------------------------------------------------------
	#include	"../library/image.h"
	#include	"../library/integer.h"
	#include	"../library/ui.h"
	#include	"../library/string.h"
	#include	"../library/window.h"
	//----------------------------------------------------------------------

#define	TEST_WIDTH_pixel	600
#define	TEST_HEIGHT_pixel	297

#define	KURO_MIMETYPE_unknown		0x00
#define	KURO_MIMETYPE_up		0x01
#define	KURO_MIMETYPE_directory		0x02
#define	KURO_MIMETYPE_link		0x03
#define	KURO_MIMETYPE_executable	0x04
#define	KURO_MIMETYPE_library		0x05
#define	KURO_MIMETYPE_module		0x06
#define	KURO_MIMETYPE_image		0x07
#define	KURO_MIMETYPE_plain_text	0x08
#define	KURO_MIMETYPE_3d_object		0x09
#define	KURO_MIMETYPE_memory		0x0A
#define	KURO_MIMETYPE_disk		0x0B
#define	KURO_MIMETYPE_log		0x0C

struct LIB_UI_STRUCTURE *ui;

uint64_t y = 0;
struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ENTRY *table_content;

uint32_t **kuro_icons = EMPTY;

MACRO_IMPORT_FILE_AS_STRING( textarea, "root/LICENSE.txt" )

uint32_t *lib_interface_icon( uint8_t *path ) {
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
	kuro_icons = (uint32_t **) realloc( kuro_icons, type * sizeof( uint32_t * ) );
	kuro_icons[ type ] = lib_image_scale( lib_interface_icon( path ), 48, 48, 16, 16 );
}

uint8_t kuro_compare_names( struct STD_STRUCTURE_DIR *first, struct STD_STRUCTURE_DIR *second ) {
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
void kuro_dir_sort( struct STD_STRUCTURE_DIR *entries, uint64_t n ) {
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
			if( kuro_compare_names( (struct STD_STRUCTURE_DIR *) &directories[ i ], (struct STD_STRUCTURE_DIR *) &directories[ i + 1 ] ) ) {
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
			if( kuro_compare_names( (struct STD_STRUCTURE_DIR *) &files[ i ], (struct STD_STRUCTURE_DIR *) &files[ i + 1 ] ) ) {
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

void create_ui( void ) {
	// column width
	uint64_t column_width = (320 - (LIB_UI_MARGIN_DEFAULT * 3)) >> STD_SHIFT_2;

	// row 0

	// column 0
	uint64_t x0 = LIB_UI_MARGIN_DEFAULT;
	uint64_t y0 = LIB_UI_HEADER_HEIGHT;
	lib_ui_add_label( ui, x0, y0, column_width, (uint8_t *) "Use TAB/SHIFT key or mouse for interaction.", EMPTY, EMPTY );
	y0 += LIB_UI_ELEMENT_LABEL_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_input( ui, x0, y0, column_width, (uint8_t *) "Input.0", EMPTY, EMPTY );
	y0 += LIB_UI_ELEMENT_INPUT_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_checkbox( ui, x0, y0, column_width, (uint8_t *) "Checkbox.0", EMPTY );
	y0 += LIB_UI_ELEMENT_CHECKBOX_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_checkbox( ui, x0, y0, column_width, (uint8_t *) "Checkbox.1", LIB_UI_ELEMENT_FLAG_set );
	y0 += LIB_UI_ELEMENT_CHECKBOX_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_button( ui, x0, y0, column_width, (uint8_t *) "Button.0", LIB_UI_ELEMENT_BUTTON_height, EMPTY );
	y0 += LIB_UI_ELEMENT_BUTTON_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_button( ui, x0, y0, column_width, (uint8_t *) "Button.1", LIB_UI_ELEMENT_BUTTON_height, EMPTY );
	y0 += LIB_UI_ELEMENT_BUTTON_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_button( ui, x0, y0, column_width, (uint8_t *) "Button.2 (disabled)", LIB_UI_ELEMENT_BUTTON_height, LIB_UI_ELEMENT_FLAG_disabled );

	// column 1
	uint64_t x1 = LIB_UI_MARGIN_DEFAULT + column_width + LIB_UI_PADDING_DEFAULT;
	uint64_t y1 = LIB_UI_HEADER_HEIGHT + LIB_UI_ELEMENT_LABEL_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_input( ui, x1, y1, column_width, (uint8_t *) "Input.1 (disabled)", LIB_UI_ELEMENT_FLAG_disabled, EMPTY );
	y1 += LIB_UI_ELEMENT_INPUT_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_radio( ui, x1, y1, column_width, (uint8_t *) "Radio.0 (Group.0)", 0, EMPTY );
	y1 += LIB_UI_ELEMENT_RADIO_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_radio( ui, x1, y1, column_width, (uint8_t *) "Radio.1 (Group.0)", 0, EMPTY );
	y1 += LIB_UI_ELEMENT_RADIO_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_radio( ui, x1, y1, column_width, (uint8_t *) "Radio.2 (Group.0)", 0, LIB_UI_ELEMENT_FLAG_set );
	y1 += LIB_UI_ELEMENT_RADIO_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_radio( ui, x1, y1, column_width, (uint8_t *) "Radio.3 (Group.0)", 0, EMPTY );
	y1 += LIB_UI_ELEMENT_RADIO_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_radio( ui, x1, y1, column_width, (uint8_t *) "Radio.0 (Group.1)", 1, EMPTY );
	y1 += LIB_UI_ELEMENT_RADIO_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_radio( ui, x1, y1, column_width, (uint8_t *) "Radio.1 (Group.1)", 1, LIB_UI_ELEMENT_FLAG_set );
	y1 += LIB_UI_ELEMENT_RADIO_height + LIB_UI_PADDING_DEFAULT;

	// column 2
	uint64_t x2 = LIB_UI_MARGIN_DEFAULT + column_width + LIB_UI_PADDING_DEFAULT + column_width + LIB_UI_PADDING_DEFAULT;
	uint64_t y2 = LIB_UI_HEADER_HEIGHT;
	struct STD_STRUCTURE_DIR *dir = (struct STD_STRUCTURE_DIR *) std_dir( (uint8_t *) ".", TRUE );
	uint64_t f_count = EMPTY; while( dir[ ++f_count ].type );
	kuro_dir_sort( (struct STD_STRUCTURE_DIR *) &dir[ 1 ], f_count - 1 );
	struct LIB_UI_STRUCTURE_ELEMENT_TABLE_HEADER *table_header = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE_HEADER *) malloc( 2 * sizeof( struct LIB_UI_STRUCTURE_ELEMENT_TABLE_HEADER ) );
	// header
		// subcolumn 0
		table_header[ 0 ].width	= EMPTY;
		table_header[ 0 ].cell.flag	= LIB_FONT_FLAG_WEIGHT_bold;
		table_header[ 0 ].cell.name	= (uint8_t *) calloc( 5 );
		uint8_t name[] = "Name:";
		for( uint8_t i = 0; i < sizeof( name ) - 1; i++ ) table_header[ 0 ].cell.name[ i ] = name[ i ];
		table_header[ 0 ].cell.icon	= EMPTY;
		// subcolumn 1
		table_header[ 1 ].width	= EMPTY;
		table_header[ 1 ].cell.flag	= LIB_FONT_FLAG_WEIGHT_bold | LIB_FONT_FLAG_ALIGN_right;
		table_header[ 1 ].cell.name	= (uint8_t *) calloc( 5 );
		uint8_t size[] = "Bytes:";
		for( uint8_t i = 0; i < sizeof( size ) - 1; i++ ) table_header[ 1 ].cell.name[ i ] = size[ i ];
		table_header[ 1 ].cell.icon	= EMPTY;
	// rows
	table_content = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ENTRY *) malloc( FALSE );
	uint64_t f = 1;
	while( dir[ ++f ].type ) {
		table_content = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ENTRY *) realloc( table_content, (y + 1) * sizeof( struct LIB_UI_STRUCTURE_ELEMENT_TABLE_ENTRY ) );
		//---
		table_content[ y ].cell = (struct LIB_UI_STRUCTURE_ELEMENT_TABLE_CELL *) malloc( 2 * sizeof( struct LIB_UI_STRUCTURE_ELEMENT_TABLE_CELL ) );
		table_content[ y ].flag = EMPTY;
		table_content[ y ].cell[ 0 ].flag = EMPTY;
		table_content[ y ].cell[ 0 ].name = EMPTY;
		table_content[ y ].cell[ 0 ].icon = kuro_icons[ KURO_MIMETYPE_unknown ];
		if( dir[ f ].type & STD_FILE_TYPE_link ) table_content[ y ].cell[ 0 ].icon = kuro_icons[ KURO_MIMETYPE_up ];
		else {
			table_content[ y ].cell[ 0 ].name = (uint8_t *) calloc( dir[ f ].name_limit + TRUE );
			for( uint64_t i = 0; i < dir[ f ].name_limit; i++ ) table_content[ y ].cell[ 0 ].name[ i ] = dir[ f ].name[ i ];
		}
		if( dir[ f ].type & STD_FILE_TYPE_directory ) table_content[ y ].cell[ 0 ].icon = kuro_icons[ KURO_MIMETYPE_directory ];
		if( dir[ f ].type & STD_FILE_TYPE_file ) table_content[ y ].cell[ 0 ].icon = kuro_icons[ KURO_MIMETYPE_plain_text ];
		//---
		table_content[ y ].cell[ 1 ].flag = LIB_FONT_FLAG_ALIGN_right;
		if( dir[ f ].type & STD_FILE_TYPE_link || dir[ f ].type & STD_FILE_TYPE_directory ) table_content[ y ].cell[ 1 ].name = EMPTY;
		else {
			table_content[ y ].cell[ 1 ].name = (uint8_t *) calloc( lib_integer_digit_count( dir[ f ].limit, STD_NUMBER_SYSTEM_decimal ) + TRUE );
			lib_integer_to_string( dir[ f ].limit, STD_NUMBER_SYSTEM_decimal, table_content[ y ].cell[ 1 ].name );
		}
		table_content[ y ].cell[ 1 ].icon = EMPTY;
		//---
		y++;
	}
	lib_ui_add_table( ui, x2, y2, -1, y1 - (LIB_UI_HEADER_HEIGHT + LIB_UI_PADDING_DEFAULT), table_header, table_content, 2, y );

	// row 1

	// column 0
	y1 += LIB_UI_ELEMENT_RADIO_height + LIB_UI_PADDING_DEFAULT;
	lib_ui_add_textarea( ui, x0, y1, -1, -1, LIB_UI_ELEMENT_FLAG_disabled, (uint8_t *) &textarea, LIB_FONT_FAMILY_ROBOTO );

	lib_window_name( ui -> window, (uint8_t *) "GUI Debug Window" );

	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_close );
	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_max );
	lib_ui_add_control( ui, LIB_UI_ELEMENT_CONTROL_TYPE_min );

	lib_ui_flush( ui );
}

uint64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// always start from root directory
	std_cd( (uint8_t *) "/", TRUE );

	// initialize icon list
	kuro_icons = (uint32_t **) malloc( TRUE );

	// register initial icon (directory change)
	kuro_icon_register( KURO_MIMETYPE_up, (uint8_t *) "/var/share/media/icon/default/empty.tga" );
	kuro_icon_register( KURO_MIMETYPE_plain_text, (uint8_t *) "/var/share/media/icon/default/mimetypes/text-plain.tga" );
	kuro_icon_register( KURO_MIMETYPE_directory, (uint8_t *) "/var/share/media/icon/default/places/folder.tga" );
	kuro_icon_register( KURO_MIMETYPE_unknown, (uint8_t *) "/var/share/media/icon/default/mimetypes/unknown.tga" );

	struct LIB_WINDOW_STRUCTURE *window = lib_window( -1, -1, TEST_WIDTH_pixel, TEST_HEIGHT_pixel );
	// struct LIB_WINDOW_STRUCTURE *window = lib_window( 3, 289, TEST_WIDTH_pixel, TEST_HEIGHT_pixel );
	window -> flags = LIB_WINDOW_FLAG_resizable;

	ui = lib_ui( window );

	ui -> icon = lib_image_scale( lib_icon_icon( (uint8_t *) "/var/share/media/icon/default/app/duckstation.tga" ), 48, 48, 16, 16 );

	// add icon to window properties
	for( uint64_t i = 0; i < 16 * 16; i++ ) window -> icon[ i ] = ui -> icon[ i ];

	lib_ui_clean( ui );

	create_ui();

	ui -> window -> flags |= LIB_WINDOW_FLAG_visible | LIB_WINDOW_FLAG_icon | LIB_WINDOW_FLAG_flush;

	while( TRUE ) {
		uint16_t key = lib_ui_event( ui );
		
		if( key == STD_ASCII_ESC ) return EMPTY;

		sleep( TRUE );
	}

	return 0;
}
