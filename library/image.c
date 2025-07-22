/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_IMAGE
		#include	"./image.h"
	#endif

void lib_image_blur( uint32_t *image, uint8_t level, uint64_t width, uint64_t height ) {
	// temporary image space
	uint32_t *tmp = (uint32_t *) std_memory_alloc( MACRO_PAGE_ALIGN_UP( ( width * height ) << STD_VIDEO_DEPTH_shift ) >> STD_SHIFT_PAGE );

	// for every pixel in row
	for (int y = 0; y < height; y++) {
		// and column
		for (int x = 0; x < width; x++) {
			// initialize ratios [a]lpha, [r]ed, [g]reen, [b]lue, [p]ixels
			uint64_t a = 0, r = 0, g = 0, b = 0, p = 0;

			// add up all individual colors for current pixel, and all around him
			for( int zy = y - level; zy < y + level; zy++ ) {
				for( int zx = x - level; zx < x + level; zx++ ) {
					// if pixel exist
					if( zy >= 0 && zy < height && zx >= 0 && zx < width ) {
						// alpha
						a += (image[ (zy * width) + zx ] >> 24) & 0x00FF;
						// red
						r += (image[ (zy * width) + zx ] >> 16) & 0x00FF;
						// green
						g += (image[ (zy * width) + zx ] >> 8) & 0x00FF;
						// blue
						b += (image[ (zy * width) + zx ]) & 0x00FF;

						// amount of pixels for current color
						p++;
					}
				}
			}

			// combine new color
			tmp[ (y * width) + x ] = ((a/p) & 0x00FF) << 24 | ((r/p) & 0x00FF) << 16 | ((g/p) & 0x00FF) << 8 | ((b/p) & 0x00FF);
		}
	}

	// update
	for( uint64_t i = 0; i < width * height; i++ ) image[ i ] = tmp[ i ];

	// release temporary space
	std_memory_release( (uintptr_t) tmp, MACRO_PAGE_ALIGN_UP( ( width * height ) << STD_VIDEO_DEPTH_shift ) >> STD_SHIFT_PAGE );
}

uint32_t *lib_icon_icon( uint8_t *path ) {
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

uint32_t *lib_image_scale( uint32_t *source, uint64_t source_width, uint64_t source_height, uint64_t target_width, uint64_t target_height ) {
	// allocate new image area
	uint32_t *target = (uint32_t *) calloc( (target_width * target_height) << STD_VIDEO_DEPTH_shift );

	// calculate scale factors
	double x_scale_factor = (double) ((double) source_width / (double) target_width);
	double y_scale_factor = (double) ((double) source_height / (double) target_height);

	// convert image
	for( uint16_t y = 0; y < target_height; y++ ) for( uint16_t x = 0; x < target_width; x++ ) target[ (y * target_height) + x ] = source[ (uint64_t) (((uint64_t) (y_scale_factor * y) * source_height) + (uint64_t) (x * x_scale_factor)) ];

	// release source
	free( source );

	// done
	return target;
}

uint8_t lib_image_tga_parse( uint8_t *source, uint32_t *target, uint64_t bytes ) {
	int i, j, k, x, y, w = (source[ 13 ] << 8) + source[ 12 ], h = (source[ 15 ] << 8) + source[ 14 ], o = (source[ 11 ] << 8) + source[ 10 ];
	int m = ( (source[ 1 ] ? (source[ 7 ] >> 3) * source[ 5 ] : 0) + 18 );

	if( w < 1 || h < 1 ) return EMPTY;

	switch( source[ 2 ] ) {
		case 1: {
			if( source[ 6 ] != 0 || source[ 4 ] != 0 || source[ 3 ] != 0 || (source[ 7 ] != 24 && source[ 7 ] != 32) ) return FALSE;
			for( y = i = 0; y < h; y++ ) {
				k = ( (! o ? h - y - 1 : y) * w );
				for( x = 0; x < w; x++ ) {
					j = source[ m + k++ ] * ( source[ 7 ] >> 3 ) + 18;
					target[ i++ ] = ( (source[ 7 ] == 32 ? source[ j + 3 ] : 0xFF) << 24 ) | (source[ j + 2 ] << 16) | (source[ j + 1 ] << 8) | source[ j ];
				}
			}

			break;
		}
		case 2: {
			if( source[ 5 ] != 0 || source[ 6 ] != 0 || source[ 1 ] != 0 || (source[ 16 ] != 24 && source[ 16 ] != 32)) return FALSE;
			for( y = i = 0; y < h; y++ ) {
				j = ( (! o ? h - y - 1 : y) * w * (source[ 16 ] >> 3) );
				for( x = 0; x < w; x++ ) {
					target[ i++ ] = ( (source[ 16 ] == 32 ? source[ j + 3 ] : 0xFF) << 24 ) | (source[ j + 2 ] << 16) | (source[ j + 1 ] << 8) | source[ j ];
					j += source[ 16 ] >> 3;
				}
			}

			break;
		}
		case 9: {
			if( source[ 6 ] != 0 || source[ 4 ] != 0 || source[ 3 ] != 0 || (source[ 7 ] != 24 && source[ 7 ] != 32)) return FALSE;
			y = i = 0;
			for( x = 0; x < w * h &&  m < bytes; ) {
				k = source[ m++ ];
				if( k > 127 ) {
					k -= 127; x += k;
					j = source[ m++ ] * (source[ 7 ] >> 3) + 18;
					while( k-- ) {
						if( ! (i % w) ) { i = ( (! o ? h - y - 1 : y) * w ); y++; }
						target[ i++ ] = ( (source[ 7 ] == 32 ? source[ j + 3 ] : 0xFF) << 24 ) | (source[ j + 2 ] << 16) | (source[ j + 1 ] << 8) | source[ j ];
					}
				} else {
					k++; x += k;
					while( k-- ) {
						j = source[ m++ ] * (source[ 7 ] >> 3) + 18;
						if( ! (i % w) ) { i = ( (! o ? h - y - 1 : y) * w ); y++; }
						target[ i++ ] = ( (source[ 7 ] == 32 ? source[ j + 3 ] : 0xFF) << 24 ) | (source[ j + 2 ] << 16) | (source[ j + 1 ] << 8) | source[ j ];
					}
				}
			}

			break;
		}
		case 10: {
			if( source[ 5 ] != 0 || source[ 6 ] != 0 || source[ 1 ] != 0 || (source[ 16 ] != 24 && source[ 16 ] != 32)) return FALSE;
			y = i = 0;
			for( x = 0; x < w * h && m < bytes; ) {
				k = source[ m++ ];
				if( k > 127) {
					k -= 127; x += k;
					while( k-- ) {
						if( ! (i % w) ) { i = ( (! o ? h - y - 1 : y) * w ); y++; }
						target[ i++ ] = ( (source[ 16 ] == 32 ? source[ m + 3 ] : 0xFF) << 24) | (source[ m + 2 ] << 16) | (source[ m + 1 ] << 8) | source[ m ];
					}
					m += source[ 16 ] >> 3;
				} else {
					k++; x += k;
					while( k-- ) {
						if( ! (i % w) ) { i = ( (! o ? h - y - 1 : y) * w); y++; }
						target[ i++ ] = ( (source[ 16 ] == 32 ? source[ m + 3 ] : 0xFF) << 24) | (source[ m + 2 ] << 16) | (source[ m + 1 ] << 8) | source[ m ];
						m += source[ 16 ] >> 3;
					}
				}
			}

			break;
		}
		default:
			return FALSE;
	}

	return TRUE;
}
