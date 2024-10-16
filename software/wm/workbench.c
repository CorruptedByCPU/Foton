// /*===============================================================================
//  Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
// ===============================================================================*/

int64_t wm_workbench( void ) {
	// debug

	struct STD_STRUCTURE_FILE icon_file = { EMPTY };
	struct LIB_IMAGE_STRUCTURE_TGA *icon_image = EMPTY;
	uint8_t icon_path[] = "/system/var/gfx/icons/48-console.tga";
	if( (icon_file.socket = std_file_open( (uint8_t *) &icon_path, sizeof( icon_path ) - 1 )) ) {
		std_file( (struct STD_STRUCTURE_FILE *) &icon_file );
		icon_image = (struct LIB_IMAGE_STRUCTURE_TGA *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( icon_file.byte ) >> STD_SHIFT_PAGE );
		std_file_read( (struct STD_STRUCTURE_FILE *) &icon_file, (uint8_t *) icon_image, icon_file.byte );
	}
	struct WM_STRUCTURE_OBJECT *icon = wm_object_create( 8, 8, lib_font_length_string( LIB_FONT_FAMILY_ROBOTO, (uint8_t *) "Console", 7 ), 48 + LIB_FONT_HEIGHT_pixel );
	icon -> pid = wm_pid;
	uint32_t *icon_pixel = (uint32_t *) ((uintptr_t) icon -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR ));
	if( icon_image ) {
		uint32_t *tmp_icon_image = (uint32_t *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( icon -> size_byte ) >> STD_SHIFT_PAGE );
		lib_image_tga_parse( (uint8_t *) icon_image, tmp_icon_image, icon_file.byte );
		for( uint16_t y = 0; y < icon -> height; y++ )
			for( uint16_t x = 0; x < icon_image -> width; x++ )
				icon_pixel[ (y * lib_font_length_string( LIB_FONT_FAMILY_ROBOTO, (uint8_t *) "Console", 7 )) + x + ((lib_font_length_string( LIB_FONT_FAMILY_ROBOTO, (uint8_t *) "Console", 7 ) - 48) >> STD_SHIFT_2) ] = tmp_icon_image[ (y * icon_image -> width) + x ];
		std_memory_release( (uintptr_t) tmp_icon_image, MACRO_PAGE_ALIGN_UP( icon -> size_byte ) >> STD_SHIFT_PAGE );
		std_memory_release( (uintptr_t) icon_image, MACRO_PAGE_ALIGN_UP( icon_file.byte ) >> STD_SHIFT_PAGE );
		std_file_close( icon_file.socket );
	}
	lib_font( LIB_FONT_FAMILY_ROBOTO, (uint8_t *) "Console", 7, LIB_INTERFACE_COLOR_foreground, icon_pixel + (48 * lib_font_length_string( LIB_FONT_FAMILY_ROBOTO, (uint8_t *) "Console", 7 )), lib_font_length_string( LIB_FONT_FAMILY_ROBOTO, (uint8_t *) "Console", 7 ), EMPTY );
	icon -> descriptor -> flags |= STD_WINDOW_FLAG_fixed_z | STD_WINDOW_FLAG_visible | STD_WINDOW_FLAG_flush;
	while( TRUE ) sleep( TRUE );
	return EMPTY;
}
