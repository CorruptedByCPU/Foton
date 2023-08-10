/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_RGL
	#define	LIB_RGL

	typedef	struct VECTOR3F {
		double	x;
		double	y;
		double	z;
		double	w;
	} vector3f;

	struct	LIB_RGL_STRUCTURE {
		uint16_t	width_pixel;
		uint16_t	height_pixel;
		uint64_t	pitch_byte;
		uint32_t	*base_address;
		uint64_t	size_byte;
		uint32_t	*workbench_base_address;
		uint32_t	color_background;
	};

	struct	LIB_RGL_STRUCTURE_TRIANGLE {
		vector3f	point[ 3 ];
		double		z_depth;
		uint32_t	color;
	};

	// init function of Raw Graphis Library
	struct LIB_RGL_STRUCTURE *lib_rgl( uint16_t width_pixel, uint16_t height_pixel, uint32_t *base_address );

	// cleans worbench area
	void lib_rgl_clean( struct LIB_RGL_STRUCTURE *rgl );

	// synchronizes workbench area with framebuffer
	void lib_rgl_flush( struct LIB_RGL_STRUCTURE *rgl );
#endif