/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void object_load( void ) {
	// assign area for file structure
	FILE *file_material = (FILE *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( FILE ) ) >> STD_SHIFT_PAGE );

	// open new socket for file
	uint8_t file_material_path[] = "/system/var/model.mtl";
	file_material -> socket = std_file_open( (uint8_t *) &file_material_path, lib_string_trim( (uint8_t *) &file_material_path, lib_string_length( (uint8_t *) &file_material_path ) ) );

	// if file doesn't exist
	if( file_material -> socket < 0 ) {
		// release file pointer
		std_memory_release( (uintptr_t) file_material, MACRO_PAGE_ALIGN_UP( sizeof( FILE ) ) >> STD_SHIFT_PAGE );

		// cannot open such file
		exit();
	}

	// retrieve properties of opened file
	std_file( file_material );

	// assign area for file content
	uint8_t *file_material_start = (uint8_t *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( file_material -> byte ) >> STD_SHIFT_PAGE );
	uint8_t *file_material_end = (uint8_t *) file_material_start + file_material -> byte;

	// read file
	fread( file_material, file_material_start, file_material -> byte );

	// count materials
	uint8_t *line = file_material_start;
	while( line < file_material_end ) {
		// material definition?
		if( lib_string_compare( line, (uint8_t *) &string_material, sizeof( string_material ) ) ) mc++;

		// next line from file
		line += lib_string_length_line( line ) + 1;	// omit line feed character
	}

	// alloc area for materials
	material = (struct LIB_RGL_STRUCTURE_MATERIAL *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct LIB_RGL_STRUCTURE_MATERIAL ) * mc ) >> STD_SHIFT_PAGE );

	// register materials
	line = file_material_start;
	while( line < file_material_end ) {
		// material definition?
		if( lib_string_compare( line, (uint8_t *) &string_material, sizeof( string_material ) ) ) {
			// set string pointer to material name
			line += sizeof( string_material ) + 1;

			// retrieve material name and length
			material[ m ].length = lib_string_length_line( line );
			for( uint64_t i = 0; i < material[ m ].length; i++ ) material[ m ].name[ i ] = line[ i ];

			// parse until end of material properties
			while( lib_string_length_line( line ) ) {
				// diffusion?
				if( lib_string_compare( line, (uint8_t *) &string_kd, sizeof( string_kd ) ) ) {
					// set string pointer to first value
					uint8_t *p = (uint8_t *) line + sizeof( string_kd ) + 1;
					uint64_t pl = lib_string_word( p, lib_string_length_line( line ) );

					// Red
					material[ m ].Kd.x = strtof( p, pl ) * 255.0f;

					// set pointer at second value
					p += pl + 1;
					pl = lib_string_word( p, lib_string_length_line( line ) );

					// Green
					material[ m ].Kd.y = strtof( p, pl ) * 255.0f;

					// set pointer at third value
					p += pl + 1;
					pl = lib_string_word( p, lib_string_length_line( line ) );

					// Blue
					material[ m ].Kd.z = strtof( p, pl ) * 255.0f;
				}

				// ambient?
				if( lib_string_compare( line, (uint8_t *) &string_ka, sizeof( string_ka ) ) ) {
					// set string pointer to first value
					uint8_t *p = (uint8_t *) line + sizeof( string_ka ) + 1;
					uint64_t pl = lib_string_word( p, lib_string_length_line( line ) );

					// Red
					material[ m ].Ka.x = strtof( p, pl );

					// set pointer at second value
					p += pl + 1;
					pl = lib_string_word( p, lib_string_length_line( line ) );

					// Green
					material[ m ].Ka.y = strtof( p, pl );

					// set pointer at third value
					p += pl + 1;
					pl = lib_string_word( p, lib_string_length_line( line ) );

					// Blue
					material[ m ].Ka.z = strtof( p, pl );
				}

				// next line from file
				line += lib_string_length_line( line ) + 1;	// omit line feed character
			}

			// material registered
			m++;
		}

		// next line from file
		line += lib_string_length_line( line ) + 1;	// omit line feed character
	}

	// release file content
	std_memory_release( (uintptr_t) file_material_start, MACRO_PAGE_ALIGN_UP( file_material -> byte ) >> STD_SHIFT_PAGE );

	// close file
	std_memory_release( (uintptr_t) file_material, MACRO_PAGE_ALIGN_UP( sizeof( FILE ) ) >> STD_SHIFT_PAGE );

	// assign area for file structure
	FILE *file_object = (FILE *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( FILE ) ) >> STD_SHIFT_PAGE );

	// open new socket for file
	uint8_t file_object_path[] = "/system/var/model.obj";
	file_object -> socket = std_file_open( (uint8_t *) &file_object_path, lib_string_trim( (uint8_t *) &file_object_path, lib_string_length( (uint8_t *) &file_object_path ) ) );

	// if file doesn't exist
	if( file_object -> socket < 0 ) {
		// release file pointer
		std_memory_release( (uintptr_t) file_object, MACRO_PAGE_ALIGN_UP( sizeof( FILE ) ) >> STD_SHIFT_PAGE );

		// cannot open such file
		exit();
	}

	// retrieve properties of opened file
	std_file( file_object );

	// assign area for file content
	uint8_t *file_object_start = (uint8_t *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( file_object -> byte ) >> STD_SHIFT_PAGE );
	uint8_t *file_object_end = (uint8_t *) file_object_start + file_object -> byte;

	// read file
	fread( file_object, file_object_start, file_object -> byte );

	// count vectors and faces
	line = file_object_start;
	while( line < file_object_end ) {
		// vector?
		if( line[ 0 ] == 'v' ) vc++;

		// face?
		if( line[ 0 ] == 'f' ) fc++;

		// next line from file
		line += lib_string_length_line( line ) + 1;	// omit line feed character
	}

	// alloc area for points and faces
	vector = (vector3f *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( vector3f ) * vc ) >> STD_SHIFT_PAGE );
	vr = (vector3f *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( vector3f ) * vc ) >> STD_SHIFT_PAGE );
	vp = (vector3f *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( vector3f ) * vc ) >> STD_SHIFT_PAGE );
	face = (struct LIB_RGL_STRUCTURE_TRIANGLE *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct LIB_RGL_STRUCTURE_TRIANGLE ) * fc ) >> STD_SHIFT_PAGE );

	// register default material properties
	// material[ 0 ].Kd = STD_COLOR_WHITE;

	// by default use initial material
	uint64_t material_id = 0;

	// register vectors and faces
	line = file_object_start;
	while( line < file_object_end ) {
		// vector?
		if( *line == 'v' ) {
			// set point at first value
			uint8_t *vs = (uint8_t *) &line[ 2 ];
			uint64_t vl = lib_string_word( vs, lib_string_length_line( line ) );

			// X axis
			vector[ v ].x = strtof( vs, vl );

			// set pointer at second value
			vs += vl + 1;
			vl = lib_string_word( vs, lib_string_length_line( line ) );

			// Y axis
			vector[ v ].y = strtof( vs, vl );

			// set pointer at third value
			vs += vl + 1;
			vl = lib_string_word( vs, lib_string_length_line( line ) );

			// Z axis
			vector[ v ].z = strtof( vs, vl );

			// W as default
			vector[ v ].w = 1.0f;

			// vector registered
			v++;
		}

		// change material?
		if( lib_string_compare( line, (uint8_t *) string_material_change, sizeof( string_material_change ) ) ) {
			// set pointer to material name
			line += sizeof( string_material_change ) + 1;

			// retrieve material id
			for( uint64_t i = 0; i < mc; i++ )
				// material name found?
				if( lib_string_compare( line, (uint8_t *) &material[ i ].name, lib_string_length_line( line ) ) )
					// yes
					material_id = i;
		}

		// face?
		if( *line == 'f' ) {
			// set pointer at first value
			uint8_t *fs = (uint8_t *) &line[ 2 ];
			uint64_t fl = lib_string_length_scope_digit( fs );

			// first point of face
			face[ f ].v[ 0 ] = (uint64_t) lib_string_to_integer( fs, 10 );

			// set pointer at second value
			fs += fl + 1;
			fl = lib_string_length_scope_digit( fs );

			// second point of face
			face[ f ].v[ 1 ] = (uint64_t) lib_string_to_integer( fs, 10 );

			// set pointer at third value
			fs += fl + 1;
			fl = lib_string_length_scope_digit( fs );

			// third point of face
			face[ f ].v[ 2 ] = (uint64_t) lib_string_to_integer( fs, 10 );

			// use definied material
			face[ f ].material = material_id;

			// face registered
			f++;
		}

		// next line from file
		line += lib_string_length_line( line ) + 1;	// omit line feed character
	}

	// release file content
	std_memory_release( (uintptr_t) file_object_start, MACRO_PAGE_ALIGN_UP( file_object -> byte ) >> STD_SHIFT_PAGE );

	// close file
	std_memory_release( (uintptr_t) file_object, MACRO_PAGE_ALIGN_UP( sizeof( FILE ) ) >> STD_SHIFT_PAGE );
}