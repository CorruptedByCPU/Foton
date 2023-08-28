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

	// prepare workbench area
	rgl -> depth_base_address = (double *) malloc( rgl -> width_pixel * rgl -> height_pixel * sizeof( double ) );


	// set default color of RGL area
	rgl -> color_background = STD_COLOR_BLACK;

	// camera position
	rgl -> camera.x = 0.0f;
	rgl -> camera.y = 0.0f;
	rgl -> camera.z = -5.0f;

	// and its target
	rgl -> target.x = 0.0f;
	rgl -> target.y = 0.0f;
	rgl -> target.z = 0.0f;

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

	double d;
	(*((uint64_t *) &d))= ~(1LL << 52);

	// clean up depth area
	for( uint64_t y = 0; y < rgl -> height_pixel; y++ )
		for( uint64_t x = 0; x < rgl -> width_pixel; x++ )
			rgl -> depth_base_address[ (y * rgl -> width_pixel) + x ] = d;
}

void lib_rgl_flush( struct LIB_RGL_STRUCTURE *rgl ) {
	// synchronize standard output with workbench
	for( uint64_t y = 0; y < rgl -> height_pixel; y++ )
		for( uint64_t x = 0; x < rgl -> width_pixel; x++ )
			rgl -> base_address[ (y * rgl -> width_pixel) + x ] = rgl -> workbench_base_address[ (y * rgl -> width_pixel) + x ];
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
	if( low < high ) {
		uint64_t pi = 0;
		pi = lib_rgl_partition( triangles, low, high );

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

	int64_t dx = abs( x1 - x0 ), sx = (x0 < x1) ? 1 : -1;
	int64_t dy = abs( y1 - y0 ), sy = (y0 < y1) ? 1 : -1; 
	int64_t err = dx - dy;

	while( TRUE ) {
		rgl -> workbench_base_address[ (y0 * rgl -> width_pixel) + x0 ] = color;

		if( (x0 == x1) && (y0 == y1) ) break;

		int64_t e2 = err << 1;
	
		if( e2 > -dy ) { err -= dy; x0 += sx; }
		if( e2 < dx ) { err += dx; y0 += sy; }
	}
}

uint32_t lib_rgl_color( uint32_t argb, double light ) {
	double red = (double) ((argb & 0x00FF0000) >> 16);
	double green = (double) ((argb & 0x0000FF00) >> 8);
	double blue = (double) (argb & 0x000000FF);

	if( light < 0.0f ) {
		light += 1.0f;
        
		red *= light;
        	green *= light;
		blue *= light;
    	} else {
	        red = (255.0f - red) * light + red;
	        green = (255.0f - green) * light + green;
	        blue = (255.0f - blue) * light + blue;
	}

	return STD_COLOR_mask | (((uint32_t) red) << 16) | (((uint32_t) green) << 8) | ((uint32_t) blue);
}

struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_multiply_matrix( struct LIB_RGL_STRUCTURE_MATRIX this, struct LIB_RGL_STRUCTURE_MATRIX via ) {
	struct LIB_RGL_STRUCTURE_MATRIX tmp;

	for (int y = 0; y < 4; y++)
		for (int x = 0; x < 4; x++)
			tmp.cell[ x ][ y ] = this.cell[ x ][ 0 ] * via.cell[ 0 ][ y ] + this.cell[ x ][ 1 ] * via.cell[ 1 ][ y ] + this.cell[ x ][ 2 ] * via.cell[ 2 ][ y ] + this.cell[ x ][ 3 ] * via.cell[ 3 ][ y ];

	return tmp;
}

struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_empty() {
	struct LIB_RGL_STRUCTURE_MATRIX matrix = { 0.0f };

	return matrix;
}

struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_identity() {
	struct LIB_RGL_STRUCTURE_MATRIX matrix;

	matrix.cell[ 0 ][ 0 ] = 1.0f;
	matrix.cell[ 1 ][ 1 ] = 1.0f;
	matrix.cell[ 2 ][ 2 ] = 1.0f;
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

struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_scale( double x, double y, double z ) {
	struct LIB_RGL_STRUCTURE_MATRIX matrix = lib_rgl_return_matrix_identity();

	matrix.cell[ 0 ][ 0 ] = x;
	matrix.cell[ 1 ][ 1 ] = y;
	matrix.cell[ 2 ][ 2 ] = z;

	return matrix;
}

double lib_rgl_vector_product_dot( vector3f v0, vector3f v1 ) {
	return (v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z);
}

vector3f lib_rgl_vector_product_cross( vector3f v0, vector3f v1 ) {
	vector3f tmp;

	tmp.x = (v0.y * v1.z) - (v0.z * v1.y);
	tmp.y = (v0.z * v1.x) - (v0.x * v1.z);
	tmp.z = (v0.x * v1.y) - (v0.y * v1.x);

	return tmp;
}

double lib_rgl_vector_length( vector3f v ) {
	return (double) sqrtf( (float) lib_rgl_vector_product_dot( v, v ) );
}

vector3f lib_rgl_return_vector_normalize( vector3f v ) {
	double length = 1.0f / lib_rgl_vector_length( v );

	vector3f tmp;

	tmp.x = v.x * length;
	tmp.y = v.y * length;
	tmp.z = v.z * length;

	return tmp;
}

vector3f lib_rgl_vector_substract( vector3f from, vector3f substract ) {
	vector3f tmp;

	tmp.x = from.x - substract.x;
	tmp.y = from.y - substract.y;
	tmp.z = from.z - substract.z;

	return tmp;
}

// void lib_rgl_multiply( struct LIB_RGL_STRUCTURE_TRIANGLE *triangle, struct LIB_RGL_STRUCTURE_MATRIX *matrix ) {
// 	// temporary face
// 	struct LIB_RGL_STRUCTURE_TRIANGLE tmp;

// 	// for each point in face
// 	for( uint8_t i = 0; i < 3; i++ ) {
// 		// preserve face properties
// 		tmp.point[ i ] = triangle -> point[ i ];

// 		// convert by matrix
// 		triangle -> point[ i ].x = (tmp.point[ i ].x * matrix -> cell[ 0 ][ 0 ]) + (tmp.point[ i ].y * matrix -> cell[ 1 ][ 0 ]) + (tmp.point[ i ].z * matrix -> cell[ 2 ][ 0 ]) + matrix -> cell[ 3 ][ 0 ];
// 		triangle -> point[ i ].y = (tmp.point[ i ].x * matrix -> cell[ 0 ][ 1 ]) + (tmp.point[ i ].y * matrix -> cell[ 1 ][ 1 ]) + (tmp.point[ i ].z * matrix -> cell[ 2 ][ 1 ]) + matrix -> cell[ 3 ][ 1 ];
// 		triangle -> point[ i ].z = (tmp.point[ i ].x * matrix -> cell[ 0 ][ 2 ]) + (tmp.point[ i ].y * matrix -> cell[ 1 ][ 2 ]) + (tmp.point[ i ].z * matrix -> cell[ 2 ][ 2 ]) + matrix -> cell[ 3 ][ 2 ];
	
// 		double w = (tmp.point[ i ].x * matrix -> cell[ 0 ][ 3 ]) + (tmp.point[ i ].y * matrix -> cell[ 1 ][ 3 ]) + (tmp.point[ i ].z * matrix -> cell[ 2 ][ 3 ]) + matrix -> cell[ 3 ][ 3 ];

// 		if( w != 0.0f ) {
// 			triangle -> point[ i ].x /= w;
// 			triangle -> point[ i ].y /= w;
// 			triangle -> point[ i ].z /= w;
// 		}
// 	}
// }

void lib_rgl_triangle( struct LIB_RGL_STRUCTURE *rgl, struct LIB_RGL_STRUCTURE_TRIANGLE *t, vector3f *vp, struct LIB_RGL_STRUCTURE_MATERIAL *material ) {
	uint32_t color = lib_rgl_color( material[ t -> material ].Kd, t -> light );

	vector2d p0 = { (int64_t) (vp[ t -> v[ 0 ] ].x * (double) rgl -> width_pixel) + (rgl -> width_pixel >> 1), (int64_t) (vp[ t -> v[ 0 ] ].y * (double) rgl -> height_pixel) + (rgl -> height_pixel >> 1) };
	vector2d p1 = { (int64_t) (vp[ t -> v[ 1 ] ].x * (double) rgl -> width_pixel) + (rgl -> width_pixel >> 1), (int64_t) (vp[ t -> v[ 1 ] ].y * (double) rgl -> height_pixel) + (rgl -> height_pixel >> 1) };
	vector2d p2 = { (int64_t) (vp[ t -> v[ 2 ] ].x * (double) rgl -> width_pixel) + (rgl -> width_pixel >> 1), (int64_t) (vp[ t -> v[ 2 ] ].y * (double) rgl -> height_pixel) + (rgl -> height_pixel >> 1) };

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
			if( p.x < 0 || p.x > rgl -> width_pixel ) continue;	// no
			if( p.y < 0 || p.y > rgl -> height_pixel ) continue;	// no

			// pixel inside triangle?
			if( ! lib_rgl_edge( (vector2d *) &p1, (vector2d *) &p2, (vector2d *) &p ) ) continue;
			if( ! lib_rgl_edge( (vector2d *) &p2, (vector2d *) &p0, (vector2d *) &p ) ) continue;
			if( ! lib_rgl_edge( (vector2d *) &p0, (vector2d *) &p1, (vector2d *) &p ) ) continue;

			// draw it
			rgl -> workbench_base_address[ (p.y * rgl -> width_pixel) + p.x ] = color;
		}

	lib_rgl_line( rgl, p0.x, p0.y, p1.x, p1.y, color );
	lib_rgl_line( rgl, p1.x, p1.y, p2.x, p2.y, color );
	lib_rgl_line( rgl, p2.x, p2.y, p0.x, p0.y, color );
}

struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_view( struct LIB_RGL_STRUCTURE *rgl ) {
	vector3f z = lib_rgl_vector_substract( rgl -> target, rgl -> camera );
	z = lib_rgl_return_vector_normalize( z );

	vector3f x = lib_rgl_vector_product_cross( (vector3f) { 0.0f, 1.0f, 0.0f }, z );
	x = lib_rgl_return_vector_normalize( x );

	vector3f y = lib_rgl_vector_product_cross( z, x );
	y = lib_rgl_return_vector_normalize( y );

	double dx = lib_rgl_vector_product_dot( x, rgl -> camera );
	double dy = lib_rgl_vector_product_dot( y, rgl -> camera );
	double dz = lib_rgl_vector_product_dot( z, rgl -> camera );

	struct LIB_RGL_STRUCTURE_MATRIX matrix = lib_rgl_return_matrix_identity();

	matrix.cell[ 0 ][ 0 ] = x.x;
	matrix.cell[ 0 ][ 1 ] = y.x;
	matrix.cell[ 0 ][ 2 ] = z.x;
	matrix.cell[ 1 ][ 0 ] = x.y;
	matrix.cell[ 1 ][ 1 ] = y.y;
	matrix.cell[ 1 ][ 2 ] = z.y;
	matrix.cell[ 2 ][ 0 ] = x.z;
	matrix.cell[ 2 ][ 1 ] = y.z;
	matrix.cell[ 2 ][ 2 ] = z.z;
	matrix.cell[ 3 ][ 0 ] = -dx;
	matrix.cell[ 3 ][ 1 ] = -dy;
	matrix.cell[ 3 ][ 2 ] = -dz;

	return matrix;
};

uint8_t lib_rgl_projection( struct LIB_RGL_STRUCTURE *rgl, vector3f *vr, struct LIB_RGL_STRUCTURE_TRIANGLE *parse ) {
	vector3f line1 = lib_rgl_vector_substract( vr[ parse -> v[ 1 ] ], vr[ parse -> v[ 0 ] ] );
	vector3f line2 = lib_rgl_vector_substract( vr[ parse -> v[ 2 ] ], vr[ parse -> v[ 0 ] ] );

	vector3f normal = lib_rgl_vector_product_cross( line1, line2 );

	normal = lib_rgl_return_vector_normalize( normal );

	vector3f camera_ray = lib_rgl_vector_substract( vr[ parse -> v[ 0 ] ], rgl -> camera );

	// show only visible triangles
	if( lib_rgl_vector_product_dot( normal, camera_ray ) < 0.0f ) {
	// if( normal.z < 0.0f ) {
		// light source position
		vector3f light = { -1.0f, -1.0f, -1.0f };
		light = lib_rgl_return_vector_normalize( light );

		// dot product
		parse -> light = maxf( 0.1f, lib_rgl_vector_product_dot( normal, light ) ) / 4.0f;

		// triangle visible
		return TRUE;
	}

	// triangle invisible
	return FALSE;
}

struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_perspective( struct LIB_RGL_STRUCTURE *rgl, double fov, double aspect, double n, double f ) {
	struct LIB_RGL_STRUCTURE_MATRIX matrix = lib_rgl_return_matrix_empty();

	// double t = 1.0f / tan( fov * 0.5f );
	double t = 2.4327649740660564f;

	matrix.cell[ 0 ][ 0 ] = t / aspect;
	matrix.cell[ 1 ][ 1 ] = t;
	matrix.cell[ 2 ][ 2 ] = -f / (n - f);
	matrix.cell[ 2 ][ 3 ] = 1.0f;
	matrix.cell[ 3 ][ 2 ] = (n * f) / (n - f);
	matrix.cell[ 3 ][ 3 ] = 0.0f;

	return matrix;
};

struct LIB_RGL_STRUCTURE_MATRIX lib_rgl_return_matrix_projection( struct LIB_RGL_STRUCTURE *rgl ) {
	struct LIB_RGL_STRUCTURE_MATRIX matrix = lib_rgl_return_matrix_identity();

	double n = 0.1f;
	double f = 1000.0f;
	double fov = 90.0f;
	double r = 1.0f / tan( (fov * 0.5f) / (LIB_MATH_PI * 180.0f) );

	matrix.cell[ 0 ][ 0 ] = r;
	matrix.cell[ 1 ][ 1 ] = r;
	matrix.cell[ 2 ][ 2 ] = f / (f - n);
	matrix.cell[ 2 ][ 3 ] = 1.0f;
	matrix.cell[ 3 ][ 2 ] = (-f * n) / (f - n);
	matrix.cell[ 3 ][ 3 ] = 0.0f;

	return matrix;
}

vector3f lib_rgl_multiply_vector( vector3f v, struct LIB_RGL_STRUCTURE_MATRIX matrix ) {
	// temporary vector
	vector3f t;

	// convert by matrix
	t.x = (v.x * matrix.cell[ 0 ][ 0 ]) + (v.y * matrix.cell[ 1 ][ 0 ]) + (v.z * matrix.cell[ 2 ][ 0 ]) + matrix.cell[ 3 ][ 0 ];
	t.y = (v.x * matrix.cell[ 0 ][ 1 ]) + (v.y * matrix.cell[ 1 ][ 1 ]) + (v.z * matrix.cell[ 2 ][ 1 ]) + matrix.cell[ 3 ][ 1 ];
	t.z = (v.x * matrix.cell[ 0 ][ 2 ]) + (v.y * matrix.cell[ 1 ][ 2 ]) + (v.z * matrix.cell[ 2 ][ 2 ]) + matrix.cell[ 3 ][ 2 ];

	double w = (v.x * matrix.cell[ 0 ][ 3 ]) + (v.y * matrix.cell[ 1 ][ 3 ]) + (v.z * matrix.cell[ 2 ][ 3 ]) + matrix.cell[ 3 ][ 3 ];

	if( w != 0.0f ) {
		t.x /= w;
		t.y /= w;
		t.z /= w;
	}

	return t;
}