/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_RGL
		#include	"./rgl.h"
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
	rgl -> color_background = STD_COLOR_RED_light;

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