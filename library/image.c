/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions
	//----------------------------------------------------------------------
	#ifndef	LIB_IMAGE
		#include	"./image.h"
	#endif

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
