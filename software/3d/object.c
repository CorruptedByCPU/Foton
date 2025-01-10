/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void object_load( uint64_t argc, uint8_t *argv[] ) {
	// properties of object/material file
	FILE *file;
	uint8_t *object_content; uint64_t object_byte = EMPTY;
	uint8_t *material_content; uint64_t material_byte = EMPTY;

	//----------------------------------------------------------------------

	// some arguments provided?
	if( argc > 1 ) {	// yes
		// object file?
		if( lib_string_compare( (uint8_t *) &argv[ TRUE ][ lib_string_length( argv[ TRUE ] ) - 4 ], (uint8_t *) ".obj", 4 ) ) {	// yes
			// open object file
			if( ! (file = fopen( argv[ TRUE ] )) ) { log( "File not found!\n" ); exit(); }

			// assign area for object content
			if( ! (object_content = (uint8_t *) malloc( file -> byte )) ) { log( "No enough memory!" ); exit(); }

			// read directory content
			fread( file, object_content, file -> byte );

			// preserve object size in Bytes
			object_byte = file -> byte;

			// close file, no more needed
			fclose( file );
		}
	} else exit();

	//----------------------------------------------------------------------

	// materials should be inside file with the same name but .mtl extension

	// remove .obj extension
	argv[ TRUE ][ lib_string_length( argv[ TRUE ] ) - 4 ] = STD_ASCII_TERMINATOR;

	// compose material path
	uint8_t *path = malloc( TRUE );
	sprintf( "%s.mtl", path, argv[ TRUE ] );

	//----------------------------------------------------------------------

	// alloc area for materials
	material = (struct LIB_RGL_STRUCTURE_MATERIAL *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct LIB_RGL_STRUCTURE_MATERIAL ) * material_limit ) >> STD_SHIFT_PAGE );

	// open material file
	if( (file = fopen( path )) ) {
		// assign area for material content
		if( ! (material_content = (uint8_t *) malloc( file -> byte )) ) { log( "No enough memory!" ); exit(); }

		// read directory content
		fread( file, material_content, file -> byte );

		// preserve object size in Bytes
		material_byte = file -> byte;

		// close file, no more needed
		fclose( file );

		// count materials
		for( uint64_t i = 0; i < material_byte; i += lib_string_length_line( (uint8_t *) &material_content[ i ] ) + 1 )
			// material definition?
			if( lib_string_compare( (uint8_t *) &material_content[ i ], (uint8_t *) "newmtl", 6 ) ) material_limit++;

		// register materials
		uint64_t m = EMPTY; for( uint64_t i = 0; i < material_byte; i += lib_string_length_line( (uint8_t *) &material_content[ i ] ) + 1 ) {
			// material definition?
			if( lib_string_compare( (uint8_t *) &material_content[ i ], (uint8_t *) "newmtl", 6 ) ) {
				// set point at material name
				uint8_t *ms = (uint8_t *) &material_content[ i + 6 + 1 ];

				// retrieve material name and length
				material[ m ].length = lib_string_word( ms, lib_string_length_line( ms ) );
				for( uint64_t j = 0; j < material[ m ].length; j++ ) material[ m ].name[ j ] = ms[ j ];

				// parse until end of material properties
				for( ; lib_string_length_line( (uint8_t *) &material_content[ i ] ); i += lib_string_length_line( (uint8_t *) &material_content[ i ] ) + 1 ) {
					// diffusion?
					if( lib_string_compare( (uint8_t *) &material_content[ i ], (uint8_t *) "Kd", 2 ) ) {
						// set string pointer to first value
						uint8_t *ms = (uint8_t *) &material_content[ i ] + 2 + 1;
						uint64_t ml = lib_string_word( ms, lib_string_length_line( ms ) );

						// Red
						material[ m ].Kd.x = strtof( ms, ml ) * 255.0f;

						// set pointer at second value
						ms += ml + 1;
						ml = lib_string_word( ms, lib_string_length_line( ms ) );

						// Green
						material[ m ].Kd.y = strtof( ms, ml ) * 255.0f;

						// set pointer at third value
						ms += ml + 1;
						ml = lib_string_word( ms, lib_string_length_line( ms ) );

						// Blue
						material[ m ].Kd.z = strtof( ms, ml ) * 255.0f;
					}

					// ambient?
					if( lib_string_compare( (uint8_t *) &material_content[ i ], (uint8_t *) "Ka", 2 ) ) {
						// set string pointer to first value
						uint8_t *ms = (uint8_t *) &material_content[ i ] + 2 + 1;
						uint64_t ml = lib_string_word( ms, lib_string_length_line( ms ) );

						// Red
						material[ m ].Ka.x = strtof( ms, ml );

						// set pointer at second value
						ms += ml + 1;
						ml = lib_string_word( ms, lib_string_length_line( ms ) );

						// Green
						material[ m ].Ka.y = strtof( ms, ml );

						// set pointer at third value
						ms += ml + 1;
						ml = lib_string_word( ms, lib_string_length_line( ms ) );

						// Blue
						material[ m ].Ka.z = strtof( ms, ml );
					}
				}

				// material registered
				m++;
			}
		}

		// release file content
		free( material_content );
	}

	//----------------------------------------------------------------------

	// count vectors and faces
	for( uint64_t i = 0; i < object_byte; i += lib_string_length_line( (uint8_t *) &object_content[ i ] ) + 1 ) {
		// vector?
		if( object_content[ i ] == 'v' ) vector_limit++;

		// face?
		if( object_content[ i ] == 'f' ) face_limit++;
	}

	// alloc area for vectors and faces
	vector = (vector3f *) malloc( sizeof( vector3f ) * vector_limit );
	face = (struct LIB_RGL_STRUCTURE_TRIANGLE *) malloc( sizeof( struct LIB_RGL_STRUCTURE_TRIANGLE ) * face_limit );

	// by default use initial material
	uint64_t material_id = 0;

	// register vectors and faces
	uint64_t v = TRUE; uint64_t f = TRUE; for( uint64_t i = 0; i < object_byte; i += lib_string_length_line( (uint8_t *) &object_content[ i ] ) + 1 ) {
		// change material?
		if( lib_string_compare( (uint8_t *) &object_content[ i ], (uint8_t *) "usemtl", 6 ) ) {
			// set pointer to material name
			i += 6 + 1;

			// retrieve material id
			for( uint64_t j = 0; j < material_limit; j++ )
				// material name found?
				if( lib_string_compare( (uint8_t *) &object_content[ i ], (uint8_t *) &material[ j ].name, lib_string_length_line( (uint8_t *) &object_content[ i ] ) ) )
					// yes
					material_id = j;
			
			// done
			continue;
		}

		// vector?
		if( object_content[ i ] == 'v' ) {
			// set point at first value
			uint8_t *vs = (uint8_t *) &object_content[ i + 2 ];
			uint64_t vl = lib_string_word( vs, lib_string_length_line( (uint8_t *) &object_content[ i ] ) );

			// X axis
			vector[ v ].x = strtof( vs, vl );

			// set pointer at second value
			vs += vl + 1;
			vl = lib_string_word( vs, lib_string_length_line( (uint8_t *) &object_content[ i ] ) );

			// Y axis
			vector[ v ].y = strtof( vs, vl );

			// set pointer at third value
			vs += vl + 1;
			vl = lib_string_word( vs, lib_string_length_line( (uint8_t *) &object_content[ i ] ) );

			// Z axis
			vector[ v ].z = strtof( vs, vl );

			// W as default
			vector[ v ].w = 1.0f;

			// vector registered
			v++;

			// done
			continue;
		}

		// face?
		if( object_content[ i ] == 'f' ) {
			// set pointer at first value
			uint8_t *fs = (uint8_t *) &object_content[ i + 2 ];
			uint64_t fl = lib_string_length_scope_digit( fs );

			// first point of face
			face[ f ].v[ 0 ] = (int64_t) lib_string_to_integer( fs, 10 );

			// set pointer at second value
			fs += fl + 1;
			fl = lib_string_length_scope_digit( fs );

			// second point of face
			face[ f ].v[ 1 ] = (int64_t) lib_string_to_integer( fs, 10 );

			// set pointer at third value
			fs += fl + 1;
			fl = lib_string_length_scope_digit( fs );

			// third point of face
			face[ f ].v[ 2 ] = (int64_t) lib_string_to_integer( fs, 10 );

			// use definied material
			face[ f ].material = material_id;

			// BUT WHY, IT DOESN'T WORK!?
			// // there exist 3rd vector of face?
			// if( lib_string_length_line( fs + fl ) ) {
			// 	// set pointer at fourth value
			// 	fs += fl + 1;
			// 	fl = lib_string_length_scope_digit( fs );

			// 	// create face from 3rd + v0 + v2
			// 	face[ f + 1 ].v[ 0 ] = face[ f ].v[ 2 ];
			// 	face[ f + 1 ].v[ 1 ] = (int64_t) lib_string_to_integer( fs, 10 );
			// 	face[ f + 1 ].v[ 2 ] = face[ f ].v[ 0 ];

			// 	// use definied material
			// 	face[ f + 1 ].material = material_id;

			// 	// face registered
			// 	f++;
			// }

			// face registered
			f++;
		}
	}

	// release file content
	free( object_content );
}