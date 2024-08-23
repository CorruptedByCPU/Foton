/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint64_t fps = 0;
uint64_t fps_average = 0;
uint64_t fps_count = 1;
uint64_t fps_last = 0;

struct LIB_RGL_STRUCTURE *rgl;

uint8_t string_material[ 6 ] = "newmtl";
uint8_t string_material_change[ 6 ] = "usemtl";
uint8_t	string_kd[ 2 ] = "Kd";

// amount of materials
uint64_t m = 1;
uint64_t mc = 1;

struct LIB_RGL_STRUCTURE_MATERIAL *material;

// amount of vectors and faces inside object file
uint64_t v = 1;
uint64_t vc = 1;
uint64_t f = 1;
uint64_t fc = 1;

vector3f *vector;
vector3f *vr;
vector3f *vp;
struct LIB_RGL_STRUCTURE_TRIANGLE *face;

void wm_logo_object( void ) {
	// assign area for file structure
	FILE *file_material = (FILE *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( FILE ) ) >> STD_SHIFT_PAGE );

	// open new socket for file
	uint8_t file_material_path[] = "/system/var/3d.mtl";
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

	// release file content
	std_memory_release( (uintptr_t) file_material_start, MACRO_PAGE_ALIGN_UP( file_material -> byte ) >> STD_SHIFT_PAGE );

	// close file
	std_memory_release( (uintptr_t) file_material, MACRO_PAGE_ALIGN_UP( sizeof( FILE ) ) >> STD_SHIFT_PAGE );

	// assign area for file structure
	FILE *file_object = (FILE *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( FILE ) ) >> STD_SHIFT_PAGE );

	// open new socket for file
	uint8_t file_object_path[] = "/system/var/3d.obj";
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
	material[ 0 ].Kd = STD_COLOR_WHITE;

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

void wm_logo_init( void ) {
	// initialize RGL library
	rgl = lib_rgl( WM_LOGO_width, WM_LOGO_height, WM_LOGO_width, (uint32_t *) ((uintptr_t) wm_object_logo -> descriptor + sizeof( struct STD_STRUCTURE_WINDOW_DESCRIPTOR )) );

	// default background is transparent
	rgl -> color_background = EMPTY;

	// any object opacity
	rgl -> color_alpha = 0x04;

	// parse object properties
	wm_logo_object();
}

int64_t wm_logo( void ) {
	// initialize environment
	wm_logo_init();

	//----------------------------------------------------------------------

	// array of parsed faces
	struct LIB_RGL_STRUCTURE_TRIANGLE *parse = (struct LIB_RGL_STRUCTURE_TRIANGLE *) malloc( sizeof( struct LIB_RGL_STRUCTURE_TRIANGLE ) * fc );

	// array of faces sorted by Z axis
	struct LIB_RGL_STRUCTURE_TRIANGLE **sort = (struct LIB_RGL_STRUCTURE_TRIANGLE **) malloc( sizeof( struct LIB_RGL_STRUCTURE_TRIANGLE **) * fc );

	// array of perspective
	struct LIB_RGL_STRUCTURE_MATRIX p_matrix = lib_rgl_return_matrix_perspective( rgl, 0.78f, (double) rgl -> width_pixel / (double) rgl -> height_pixel, 0.01f, 1.0f );

	// array of view
	struct LIB_RGL_STRUCTURE_MATRIX v_matrix = lib_rgl_return_matrix_view( rgl );

	// array of world
	struct LIB_RGL_STRUCTURE_MATRIX w_matrix = lib_rgl_multiply_matrix( v_matrix, p_matrix );

	// rotation angle
	double a = 0.0f;

	// main loop
	while( TRUE ) {
		// next angle
		a += 0.10f;

		// clean workbench with default background color
		lib_rgl_clean( rgl );

		// calculate rotation matrixes
		struct LIB_RGL_STRUCTURE_MATRIX x_matrix = lib_rgl_return_matrix_rotate_x( a / 2.0f );
		struct LIB_RGL_STRUCTURE_MATRIX y_matrix = lib_rgl_return_matrix_rotate_y( a );
		struct LIB_RGL_STRUCTURE_MATRIX z_matrix = lib_rgl_return_matrix_rotate_z( a / 3.0f );

		// calculate movement matrix
		struct LIB_RGL_STRUCTURE_MATRIX t_matrix = lib_rgl_return_matrix_translate( 0.0f, 0.0f, 0.0f );

		// convert each vector
		for( uint64_t i = 1; i < vc; i++ ) {
			// converted vector
			vr[ i ] = vector[ i ];

			// by Q matrix
			lib_rgl_multiply_vector( &vr[ i ], &x_matrix );
			lib_rgl_multiply_vector( &vr[ i ], &y_matrix );
			lib_rgl_multiply_vector( &vr[ i ], &z_matrix );
			lib_rgl_multiply_vector( &vr[ i ], &t_matrix );
		}

		// amount of faces to sort
		uint64_t sc = 0;

		// for each face
		for( uint64_t i = 1; i <= fc; i++ ) {
			// check face visibility
			if( lib_rgl_projection( rgl, vr, &face[ i ] ) ) {
				// face to parse
				parse[ i ] = face[ i ];

				// parse each vector of face
				for( uint8_t p = 0; p < 3; p++ ) {
					// vector number
					uint64_t v = parse[ i ].v[ p ];

					// parsed vector
					vp[ v ] = vr[ v ];

					// by W matrix
					lib_rgl_multiply_vector( &vp[ v ], &w_matrix );
				}

				// put parsed face into sorting list
				sort[ sc ] = &parse[ i ];

				// for sorting list, calculate average value of depth for 3 points of face
				sort[ sc ] -> z_depth = (vp[ parse[ i ].v[ 0 ] ].z + vp[ parse[ i ].v[ 1 ] ].z + vp[ parse[ i ].v[ 2 ] ].z) / 3.0f;

				// sort list incremented
				sc++;
			}
		}

		// sort faces by depth
		if( sc > 1 ) lib_rgl_sort_quick( sort, sc - 1, 1 );

		// draw every triangle on workbench
		for( uint64_t i = 0; i < sc; i++ ) lib_rgl_fill( rgl, sort[ i ], vp, material );

		// synchronize workbench with window
		lib_rgl_flush( rgl );

		// tell window manager to flush window
		wm_object_logo -> descriptor -> flags |= STD_WINDOW_FLAG_flush;

		// next frame ready
		fps++;

		// slow down up to 60 FPS
		uint64_t wait = std_microtime() + (1000 / 60);
		while( wait > std_microtime() ) sleep( TRUE );
	}
}