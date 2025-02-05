/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_IMAGE
	#define LIB_IMAGE

	#define	LIB_IMAGE_TGA_ENCODING_RLE_RGB	10

	struct	LIB_IMAGE_STRUCTURE_TGA {
	  	uint8_t		magic;			// 0
	  	uint8_t		color_map;		// 1
		uint8_t		encoding;		// 2
		uint16_t	cmaporig, cmaplen;	// 3, 5
		uint8_t		cmapent;		// 7
		uint16_t	x;			// 8
		uint16_t	y;			// 10
		uint16_t	width;			// 12
	 	uint16_t	height;			// 14
		uint8_t		bpp;			// 16
		uint8_t		pixeltype;		// 17
	} __attribute__( (packed));

	void lib_image_blur( uint32_t *image, uint8_t level, uint64_t width, uint64_t height );

	uint8_t lib_image_tga_parse( uint8_t *source, uint32_t *target, uint64_t bytes );

	uint32_t *lib_image_scale( uint32_t *source, uint64_t source_width, uint64_t source_height, uint64_t target_width, uint64_t target_height );
#endif
