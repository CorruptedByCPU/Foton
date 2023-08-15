/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_RGL
		#include	"./rgl.h"
	#endif
	#ifndef	LIB_MATH
		#include	"./math.h"
	#endif

struct LIB_RGL_STRUCTURE *lib_rgl( uint16_t width_pixel, uint16_t height_pixel, uint32_t *base_address ) {
	// prepare space for RGL structure
	struct LIB_RGL_STRUCTURE *rgl = (struct LIB_RGL_STRUCTURE *) malloc( sizeof( struct LIB_RGL_STRUCTURE ) );

	// set properties of RGL
	rgl -> width_pixel = width_pixel;
	rgl -> height_pixel = height_pixel;
	rgl -> base_address = base_address;

	// calculate display area size in Bytes
	rgl -> size_byte = (rgl -> width_pixel * rgl -> height_pixel) << STD_VIDEO_DEPTH_shift;

	// prepare workbench area
	rgl -> workbench_base_address = (uint32_t *) malloc( rgl -> size_byte );

	// set default color of RGL area
	rgl -> color_background = STD_COLOR_BLACK;

	// camera position
	rgl -> camera.x = 0.0f;
	rgl -> camera.y = 0.0f;
	rgl -> camera.z = -1.0f;

	// clean up workbench
	lib_rgl_clean( rgl );

	// return RGL specification for future use
	return rgl;
}

void lib_rgl_clean( struct LIB_RGL_STRUCTURE *rgl ) {
	// clean up workbench area
	for( uint64_t y = 0; y < rgl -> height_pixel; y++ )
		for( uint64_t x = 0; x < rgl -> width_pixel; x++ )
			rgl -> workbench_base_address[ (y * rgl -> width_pixel) + x ] = rgl -> color_background;
}

void lib_rgl_flush( struct LIB_RGL_STRUCTURE *rgl ) {
	// synchronize standard output with workbench
	for( uint64_t y = 0; y < rgl -> height_pixel; y++ )
		for( uint64_t x = 0; x < rgl -> width_pixel; x++ )
			rgl -> base_address[ (y * rgl -> width_pixel) + x ] = rgl -> workbench_base_address[ (y * rgl -> width_pixel) + x ];
}

void lib_rgl_multiply( struct LIB_RGL_STRUCTURE_TRIANGLE *triangle, struct LIB_RGL_STRUCTURE_MATRIX *matrix ) {
	// temporary face
	struct LIB_RGL_STRUCTURE_TRIANGLE tmp;

	// for each point in face
	for( uint8_t i = 0; i < 3; i++ ) {
		// preserve face properties
		tmp.point[ i ] = triangle -> point[ i ];

		// convert by matrix
		triangle -> point[ i ].x = (tmp.point[ i ].x * matrix -> cell[ 0 ][ 0 ]) + (tmp.point[ i ].y * matrix -> cell[ 1 ][ 0 ]) + (tmp.point[ i ].z * matrix -> cell[ 2 ][ 0 ]) + matrix -> cell[ 3 ][ 0 ];
		triangle -> point[ i ].y = (tmp.point[ i ].x * matrix -> cell[ 0 ][ 1 ]) + (tmp.point[ i ].y * matrix -> cell[ 1 ][ 1 ]) + (tmp.point[ i ].z * matrix -> cell[ 2 ][ 1 ]) + matrix -> cell[ 3 ][ 1 ];
		triangle -> point[ i ].z = (tmp.point[ i ].x * matrix -> cell[ 0 ][ 2 ]) + (tmp.point[ i ].y * matrix -> cell[ 1 ][ 2 ]) + (tmp.point[ i ].z * matrix -> cell[ 2 ][ 2 ]) + matrix -> cell[ 3 ][ 2 ];
	
		double w = (tmp.point[ i ].x * matrix -> cell[ 0 ][ 3 ]) + (tmp.point[ i ].y * matrix -> cell[ 1 ][ 3 ]) + (tmp.point[ i ].z * matrix -> cell[ 2 ][ 3 ]) + matrix -> cell[ 3 ][ 3 ];

		if( w != 0.0f ) {
			triangle -> point[ i ].x /= w;
			triangle -> point[ i ].y /= w;
			triangle -> point[ i ].z /= w;
		}
	}
}

struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_multiply_matrix( struct LIB_RGL_STRUCTURE_MATRIX *this, struct LIB_RGL_STRUCTURE_MATRIX *via ) {
	struct LIB_RGL_STRUCTURE_MATRIX tmp;

	for (int y = 0; y < 4; y++)
		for (int x = 0; x < 4; x++)
			tmp.cell[ x ][ y ] = this -> cell[ x ][ 0 ] * via -> cell[ 0 ][ y ] + this -> cell[ x ][ 1 ] * via -> cell[ 1 ][ y ] + this -> cell[ x ][ 2 ] * via -> cell[ 2 ][ y ] + this -> cell[ x ][ 3 ] * via -> cell[ 3 ][ y ];

	return tmp;
}

uint8_t lib_rgl_projection( struct LIB_RGL_STRUCTURE *rgl, struct LIB_RGL_STRUCTURE_TRIANGLE *parse ) {
	vector3f line1 = lib_rgl_vector_substract( (vector3f *) &parse -> point[ 1 ], (vector3f *) &parse -> point[ 0 ] );
	vector3f line2 = lib_rgl_vector_substract( (vector3f *) &parse -> point[ 2 ], (vector3f *) &parse -> point[ 0 ] );

	vector3f normal = lib_rgl_vector_product_cross( (vector3f *) &line1, (vector3f *) &line2 );

	lib_rgl_vector_normalize( (vector3f *) &normal );

	vector3f camera_ray = lib_rgl_vector_substract( (vector3f *) &parse -> point[ 0 ], (vector3f *) &rgl -> camera );

	// show only visible triangles
	if( lib_rgl_vector_product_dot( (vector3f *) &normal, (vector3f *) &camera_ray ) < 0.0f ) {
		// light source position
		vector3f light = { 0.0f, 0.0f, -1.0f };
		lib_rgl_vector_normalize( (vector3f *) &light );

		// dot product
		double dp = maxf( 0.1f, lib_rgl_vector_product_dot( (vector3f *) &normal, (vector3f *) &light ) );

		// select color
		dp = (15.0f * dp) * 15;
		parse -> color = ( 0xFF000000 | (uint8_t) dp << 16 | (uint8_t) dp << 8 | (uint8_t) dp );

		// triangle visible
		return TRUE;
	}

	// triangle invisible
	return FALSE;
}

struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_identity() {
	struct LIB_RGL_STRUCTURE_MATRIX matrix;

	matrix.cell[ 0 ][ 0 ] = 1.0f;
	matrix.cell[ 1 ][ 1 ] = 1.0f;
	matrix.cell[ 2 ][ 2 ] = 1.0f;
	matrix.cell[ 3 ][ 3 ] = 1.0f;

	return matrix;
}

struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_projection( double near, double far, double fov, double aspect ) {
	struct LIB_RGL_STRUCTURE_MATRIX matrix = lib_rgl_return_matrix_identity();

	double rad = 1.0f / tan( (fov * 0.5f) / (LIB_MATH_PI * 180.0f) );

	matrix.cell[ 0 ][ 0 ] = rad;
	matrix.cell[ 1 ][ 1 ] = rad;
	matrix.cell[ 2 ][ 2 ] = -(far / (far - near));
	matrix.cell[ 2 ][ 3 ] = 1.0f;
	matrix.cell[ 3 ][ 2 ] = -(far * near) / (far - near);
	matrix.cell[ 3 ][ 3 ] = 1.0f;

	return matrix;
}

struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_rotate_x( double a ) {
	struct LIB_RGL_STRUCTURE_MATRIX matrix = lib_rgl_return_matrix_identity();

	matrix.cell[ 0 ][ 0 ] = 1.0f;
	matrix.cell[ 1 ][ 1 ] = cos( a );
	matrix.cell[ 1 ][ 2 ] = -sin( a );
	matrix.cell[ 2 ][ 1 ] = sin( a );
	matrix.cell[ 2 ][ 2 ] = cos( a );
	matrix.cell[ 3 ][ 3 ] = 1.0f;

	return matrix;
}

struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_rotate_y( double a ) {
	struct LIB_RGL_STRUCTURE_MATRIX matrix = lib_rgl_return_matrix_identity();

	matrix.cell[ 0 ][ 0 ] = cos( a );
	matrix.cell[ 0 ][ 2 ] = sin( a );
	matrix.cell[ 2 ][ 0 ] = -sin( a );
	matrix.cell[ 1 ][ 1 ] = 1.0f;
	matrix.cell[ 2 ][ 2 ] = cos( a );
	matrix.cell[ 3 ][ 3 ] = 1.0f;

	return matrix;
}

struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_rotate_z( double a ) {
	struct LIB_RGL_STRUCTURE_MATRIX matrix = lib_rgl_return_matrix_identity();

	matrix.cell[ 0 ][ 0 ] = cos( a );
	matrix.cell[ 0 ][ 1 ] = -sin( a );
	matrix.cell[ 1 ][ 0 ] = sin( a );
	matrix.cell[ 1 ][ 1 ] = cos( a );
	matrix.cell[ 2 ][ 2 ] = 1.0f;
	matrix.cell[ 3 ][ 3 ] = 1.0f;

	return matrix;
}

struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_translate( double x, double y, double z ) {
	struct LIB_RGL_STRUCTURE_MATRIX matrix = lib_rgl_return_matrix_identity();

	matrix.cell[ 3 ][ 0 ] = x;
	matrix.cell[ 3 ][ 1 ] = y;
	matrix.cell[ 3 ][ 2 ] = z;

	return matrix;
}

double lib_rgl_vector_product_dot( vector3f *v0, vector3f *v1 ) {
	return (v0 -> x * v1 -> x) + (v0 -> y * v1 -> y) + (v0 -> z * v1 -> z);
}

vector3f lib_rgl_vector_product_cross( vector3f *v0, vector3f *v1 ) {
	vector3f tmp;

	tmp.x = (v0 -> y * v1 -> z) - (v0 -> z * v1 -> y);
	tmp.y = (v0 -> z * v1 -> x) - (v0 -> x * v1 -> z);
	tmp.z = (v0 -> x * v1 -> y) - (v0 -> y * v1 -> x);

	return tmp;
}

double lib_rgl_vector_length( vector3f *v ) {
	return (double) sqrtf( (float) lib_rgl_vector_product_dot( v, v ) );
}

void lib_rgl_vector_normalize( vector3f *v ) {
	double length = lib_rgl_vector_length( v );

	v -> x /= length;
	v -> y /= length;
	v -> z /= length;
}

vector3f lib_rgl_vector_substract( vector3f *from, vector3f *substract ) {
	vector3f tmp;

	tmp.x = from -> x - substract -> x;
	tmp.y = from -> y - substract -> y;
	tmp.z = from -> z - substract -> z;

	return tmp;
}

uint64_t lib_rgl_partition( struct LIB_RGL_STRUCTURE_TRIANGLE **triangles, uint64_t low, uint64_t high ) {
	double pivot = triangles[ high ] -> z_depth;

	uint64_t i = (low - 1);

	for( uint64_t j = low; j < high; j++ ) {
		if( triangles[ j ] -> z_depth < pivot ) {
			i++;

			struct LIB_RGL_STRUCTURE_TRIANGLE *tmp = triangles[ i ];
			triangles[ i ] = triangles[ j ];
			triangles[ j ] = tmp;
		}
	}

	struct LIB_RGL_STRUCTURE_TRIANGLE *tmp = triangles[ i + 1 ];
	triangles[ i + 1 ] = triangles[ high ];
	triangles[ high ] = tmp;

	return (i + 1);
}

void lib_rgl_sort_quick( struct LIB_RGL_STRUCTURE_TRIANGLE **triangles, uint64_t low, uint64_t high ) {
	if (low < high) {
		uint64_t pi = lib_rgl_partition( triangles, low, high );

		lib_rgl_sort_quick( triangles, low, pi - 1 );
		lib_rgl_sort_quick( triangles, pi + 1, high );
	}
}

inline uint8_t lib_rgl_edge( vector2d *a, vector2d *b, vector2d *c ) {
	if( (b -> x - a -> x) * (c -> y - a -> y) - (b -> y - a -> y) * (c -> x - a -> x) < 0 ) return TRUE;
	return FALSE;
}

void lib_rgl_line( struct LIB_RGL_STRUCTURE *rgl, int64_t x0, int64_t y0, int64_t x1, int64_t y1, uint32_t color ) {
	// no cliiping
	if( x0 < 0 ) x0 = 0;
	if( y0 < 0 ) y0 = 0;
	if( x1 < 0 ) x1 = 0;
	if( y1 < 0 ) y1 = 0;
	if( x0 >= rgl -> width_pixel ) x0 = rgl -> width_pixel - 1;
	if( y0 >= rgl -> height_pixel ) y0 = rgl -> height_pixel - 1;
	if( x1 >= rgl -> width_pixel ) x1 = rgl -> width_pixel - 1;
	if( y1 >= rgl -> height_pixel ) y1 = rgl -> height_pixel - 1;

	int64_t dx = abs( x1 - x0 ), sx = x0 < x1 ? 1 : -1;
	int64_t dy = -abs( y1 - y0 ), sy = y0 < y1 ? 1 : -1; 
	int64_t err = dx + dy, e2;

	for( ; ; ) {
		rgl -> workbench_base_address[ (y0 * rgl -> width_pixel) + x0 ] = color;

		if (x0 == x1 && y0 == y1) break;

		e2 = 2 * err;
	
		if (e2 >= dy) { err += dy; x0 += sx; }
		if (e2 <= dx) { err += dx; y0 += sy; }
	}
}

void lib_rgl_triangle( struct LIB_RGL_STRUCTURE *rgl, struct LIB_RGL_STRUCTURE_TRIANGLE *triangle ) {
	vector2d p0 = { (int64_t) triangle -> point[ 0 ].x, (int64_t) triangle -> point[ 0 ].y };
	vector2d p1 = { (int64_t) triangle -> point[ 1 ].x, (int64_t) triangle -> point[ 1 ].y };
	vector2d p2 = { (int64_t) triangle -> point[ 2 ].x, (int64_t) triangle -> point[ 2 ].y };

	int64_t left = p0.x;
	if( p1.x < left ) left = p1.x;
	if( p2.x < left ) left = p2.x;

	int64_t top = p0.y;
	if( p1.y < top ) top = p1.y;
	if( p2.y < top ) top = p2.y;

	int64_t right = p0.x;
	if( p1.x > right ) right = p1.x;
	if( p2.x > right ) right = p2.x;

	int64_t bottom = p0.y;
	if( p1.y > bottom ) bottom = p1.y;
	if( p2.y > bottom ) bottom = p2.y;

	vector2d p;

	for( p.y = top; p.y <= bottom; p.y++ )
		for( p.x = left; p.x <= right; p.x++ ) {
			// pixel is inside workbench?
			int64_t x = p.x + (rgl -> width_pixel / 2);
			int64_t y = p.y + (rgl -> height_pixel / 2);
			if( x < 0 || x > rgl -> width_pixel ) continue;	// no
			if( y < 0 || y > rgl -> height_pixel ) continue;	// no

			// pixel inside triangle?
			if( ! lib_rgl_edge( (vector2d *) &p1, (vector2d *) &p2, (vector2d *) &p ) ) continue;
			if( ! lib_rgl_edge( (vector2d *) &p2, (vector2d *) &p0, (vector2d *) &p ) ) continue;
			if( ! lib_rgl_edge( (vector2d *) &p0, (vector2d *) &p1, (vector2d *) &p ) ) continue;

			// draw it
			rgl -> workbench_base_address[ (y * rgl -> width_pixel) + x ] = triangle -> color;
		}

	uint32_t color = triangle -> color;
	lib_rgl_line( rgl, p0.x + (rgl -> width_pixel / 2), p0.y + (rgl -> height_pixel / 2), p1.x + (rgl -> width_pixel / 2), p1.y + (rgl -> height_pixel / 2), color );
	lib_rgl_line( rgl, p1.x + (rgl -> width_pixel / 2), p1.y + (rgl -> height_pixel / 2), p2.x + (rgl -> width_pixel / 2), p2.y + (rgl -> height_pixel / 2), color );
	lib_rgl_line( rgl, p2.x + (rgl -> width_pixel / 2), p2.y + (rgl -> height_pixel / 2), p0.x + (rgl -> width_pixel / 2), p0.y + (rgl -> height_pixel / 2), color );
}