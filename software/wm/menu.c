/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t wm_menu_header[] = KERNEL_name" v"KERNEL_version"."KERNEL_revision"";

uint32_t *menu_icon_load( uint8_t *path ) {
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

uint64_t wm_menu( void ) {
	// retrieve our process ID
	uint64_t wm_menu_pid = std_pid();

	// initial dimension of menu window
	uint64_t menu_width	= lib_font_length_string( LIB_FONT_FAMILY_ROBOTO, (uint8_t *) &wm_menu_header, sizeof( wm_menu_header ) - 1 );
	uint64_t menu_height	= LIB_UI_ELEMENT_LABEL_height + LIB_UI_PADDING_DEFAULT;

	//----------------------------------------------------------------------

	// add entries to menu list
	struct LIB_UI_STRUCTURE_ELEMENT_LIST_ENTRY *entry = (struct LIB_UI_STRUCTURE_ELEMENT_LIST_ENTRY *) malloc( sizeof( struct LIB_UI_STRUCTURE_ELEMENT_LIST_ENTRY ) << STD_SHIFT_2 );

		uint64_t entry_pixel = EMPTY;

		// first entry
		uint8_t file_manager_name[] = "File Manager";
		uint8_t file_manager_event[] = "kuro";
		// uint8_t file_manager_shortcut[] = "Menu + f";
		entry[ 0 ].icon		= lib_image_scale( menu_icon_load( (uint8_t *) "/var/share/media/icon/default/app/system-file-manager.tga" ), 48, 48, 16, 16 );
		entry[ 0 ].name		= (uint8_t *) calloc( sizeof( file_manager_name ) ); for( uint8_t i = 0; i < sizeof( file_manager_name ); i++ ) entry[ 0 ].name[ i ] = file_manager_name[ i ];
		entry[ 0 ].event	= (uint8_t *) calloc( sizeof( file_manager_event ) ); for( uint8_t i = 0; i < sizeof( file_manager_event ); i++ ) entry[ 0 ].event[ i ] = file_manager_event[ i ];
		entry[ 0 ].shortcut	= EMPTY;
		// entry[ 0 ].shortcut	= (uint8_t *) calloc( sizeof( file_manager_shortcut ) ); for( uint8_t i = 0; i < sizeof( file_manager_shortcut ); i++ ) entry[ 0 ].shortcut[ i ] = file_manager_shortcut[ i ];;

		entry_pixel = lib_font_length_string( LIB_FONT_FAMILY_ROBOTO, (uint8_t *) &file_manager_name, sizeof( file_manager_name ) - 1 );
		if( entry[ 0 ].icon )		entry_pixel += LIB_UI_PADDING_DEFAULT + 16;
		// if( entry[ 0 ].shortcut )	entry_pixel += LIB_UI_PADDING_DEFAULT + lib_font_length_string( LIB_FONT_FAMILY_ROBOTO_MONO, (uint8_t *) &file_manager_shortcut, sizeof( file_manager_shortcut ) - 1 );

		// widest entry of menu
		if( menu_width < entry_pixel ) menu_width = entry_pixel;

		// expand by default entry height
		menu_height += LIB_UI_ELEMENT_LIST_ENTRY_height;

		// second entry
		uint8_t demo_3d_name[] = "3D Viewer";
		uint8_t demo_3d_event[] = "3d /var/share/media/obj/demo.obj";
		entry[ 1 ].icon		= lib_image_scale( menu_icon_load( (uint8_t *) "/var/share/media/icon/3d.tga" ), 48, 48, 16, 16 );
		entry[ 1 ].name		= (uint8_t *) calloc( sizeof( demo_3d_name ) ); for( uint8_t i = 0; i < sizeof( demo_3d_name ); i++ ) entry[ 1 ].name[ i ] = demo_3d_name[ i ];
		entry[ 1 ].event	= (uint8_t *) calloc( sizeof( demo_3d_event ) ); for( uint8_t i = 0; i < sizeof( demo_3d_event ); i++ ) entry[ 1 ].event[ i ] = demo_3d_event[ i ];
		entry[ 1 ].shortcut	= EMPTY;

		entry_pixel = lib_font_length_string( LIB_FONT_FAMILY_ROBOTO, (uint8_t *) &demo_3d_name, sizeof( demo_3d_name ) - 1 );
		if( entry[ 1 ].icon )		entry_pixel += LIB_UI_PADDING_DEFAULT + 16;

		// widest entry of menu
		if( menu_width < entry_pixel ) menu_width = entry_pixel;

		// expand by default entry height
		menu_height += LIB_UI_ELEMENT_LIST_ENTRY_height;

	//----------------------------------------------------------------------

	// apply margins and header
	menu_width	+= LIB_UI_MARGIN_DEFAULT << STD_SHIFT_2;
	menu_height	+= LIB_UI_MARGIN_DEFAULT << STD_SHIFT_2;

	// create menu object
	wm -> menu = wm_object_create( -1, wm -> panel -> y - menu_height + TRUE, menu_width, menu_height, LIB_WINDOW_FLAG_menu | LIB_WINDOW_FLAG_fixed_z | LIB_WINDOW_FLAG_fixed_xy );

	// required for incomming messages
	wm -> menu -> pid = wm_menu_pid;

	// object name
	uint8_t menu_name[] = "{menu}";
	for( uint8_t i = 0; i < sizeof( menu_name ); i++ ) wm -> menu -> descriptor -> name[ i ] = menu_name[ i ];

	// properties of menu area content
	uint32_t *menu_pixel = (uint32_t *) ((uintptr_t) wm -> menu -> descriptor + sizeof( struct LIB_WINDOW_STRUCTURE ));

	//----------------------------------------------------------------------

	struct LIB_UI_STRUCTURE *ui = lib_ui( wm -> menu -> descriptor );

	lib_ui_add_label( ui, LIB_UI_MARGIN_DEFAULT, LIB_UI_MARGIN_DEFAULT, wm -> menu -> width - (LIB_UI_MARGIN_DEFAULT << STD_SHIFT_2), (uint8_t *) &wm_menu_header, LIB_FONT_FLAG_ALIGN_center | LIB_FONT_FLAG_WEIGHT_bold );

	lib_ui_add_list( ui, LIB_UI_MARGIN_DEFAULT, LIB_UI_MARGIN_DEFAULT + LIB_UI_ELEMENT_LABEL_height + LIB_UI_PADDING_DEFAULT, wm -> menu -> width - (LIB_UI_MARGIN_DEFAULT << STD_SHIFT_2), wm -> menu -> height - (LIB_UI_MARGIN_DEFAULT + LIB_UI_ELEMENT_LABEL_height + LIB_UI_PADDING_DEFAULT + LIB_UI_MARGIN_DEFAULT), entry, 2 );

	lib_ui_flush( ui );

	//----------------------------------------------------------------------

	while( TRUE ) {
		lib_ui_event( ui );

		// no action
		uint8_t event = FALSE;

		// check which entry acquired action
		for( uint64_t i = 0; i < 2; i++ ) {
			// action required?
			if( entry[ i ].flag & LIB_UI_ELEMENT_FLAG_event ) {
				// execute event content
				std_exec( entry[ i ].event, lib_string_length( entry[ i ].event ), EMPTY, TRUE );

				// flag parsed
				entry[ i ].flag &= ~LIB_UI_ELEMENT_FLAG_event;

				// event acquired
				event = TRUE;
			}
		}

		// Menu window not active and visible?
		if( event || wm -> active != wm -> menu && wm -> menu -> descriptor -> flags & LIB_WINDOW_FLAG_visible ) {
			// disable flag
			wm -> menu -> descriptor -> flags &= ~LIB_WINDOW_FLAG_visible;

			// request hide
			wm -> menu -> descriptor -> flags |= LIB_WINDOW_FLAG_hide;
		}

		sleep( TRUE );
	}

	// ok
	return EMPTY;
}
