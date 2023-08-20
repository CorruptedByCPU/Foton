/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void object_load() {
	// // count materials
	uint8_t *line = (uint8_t *) &file_material_start;
	while( line < (uint8_t *) &file_material_end ) {
		// material definition?
		if( lib_string_compare( line, (uint8_t *) &string_material, sizeof( string_material ) ) ) mc++;

		// next line from file
		line += lib_string_length_line( line ) + 1;	// omit line feed character
	}

	// alloc area for materials
	material = (struct LIB_RGL_STRUCTURE_MATERIAL *) malloc( sizeof( struct LIB_RGL_STRUCTURE_MATERIAL ) * mc );

	// register materials
	line = (uint8_t *) &file_material_start;
	while( line < (uint8_t *) &file_material_end ) {
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
					// initialize color value
					material[ m ].Kd = STD_COLOR_mask;

					// set string pointer to first value
					uint8_t *p = (uint8_t *) line + sizeof( string_kd ) + 1;
					uint64_t pl = lib_string_word( p, lib_string_length_line( line ) );

					// Red
					material[ m ].Kd |= ((uint8_t) (255.0 * strtof( p, pl ))) << 16;

					// set pointer at second value
					p += pl + 1;
					pl = lib_string_word( p, lib_string_length_line( line ) );

					// Green
					material[ m ].Kd |= ((uint8_t) (255.0 * strtof( p, pl ))) << 8;

					// set pointer at third value
					p += pl + 1;
					pl = lib_string_word( p, lib_string_length_line( line ) );

					// Blue
					material[ m ].Kd |= (uint8_t) (255.0 * strtof( p, pl ));
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

	// count vectors and faces
	line = (uint8_t *) &file_object_start;
	while( line < (uint8_t *) &file_object_end ) {
		// vector?
		if( line[ 0 ] == 'v' ) vc++;

		// face?
		if( line[ 0 ] == 'f' ) fc++;

		// next line from file
		line += lib_string_length_line( line ) + 1;	// omit line feed character
	}

	// alloc area for points and faces
	vector = (vector3f *) malloc( sizeof( vector3f ) * vc );
	vr = (vector3f *) malloc( sizeof( vector3f ) * vc );
	vp = (vector3f *) malloc( sizeof( vector3f ) * vc );
	face = (struct LIB_RGL_STRUCTURE_TRIANGLE *) malloc( sizeof( struct LIB_RGL_STRUCTURE_TRIANGLE ) * fc );

	// register default material properties
	material[ 0 ].Kd = STD_COLOR_WHITE;

	// by default use initial material
	uint64_t material_id = 0;

	// register vectors and faces
	line = (uint8_t *) &file_object_start;
	while( line < (uint8_t *) &file_object_end ) {
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
}