/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// columns
	uint8_t c;

	// show default colors 0..15
	c = 0;
	for( uint8_t i = 0; i < 16; i++ ) {
		if( i == 7 || i == 10 || i == 11 || i == 14 || i == 15 ) print( "\e[38;5;0m" );
		printf( "\e[48;5;%um   %2u    \e[0m", i, i );
		if( c++ == 7 ) { print( "\n" ); c = 0; }
	}

	// show predefinied colors 16..231
	c = 0;
	for( uint8_t i = 0; i < 216; i++ ) {
		if( (i + 16 > 33 && i + 16 < 52) || (i + 16 > 69 && i + 16 < 88) || (i + 16 > 105 && i + 16 < 124) || (i + 16 > 141 && i + 16 < 160) || (i + 16 > 177 && i + 16 < 196) || (i + 16 > 213 && i + 16 < 232) ) print( "\e[38;5;0m" );
		printf( "\e[48;5;%um %3u\e[0m", i + 16, i + 16 );
		if( c++ == 17 ) { print( "\n" ); c = 0; }
	}

	// show gray scale 232..255
	c = 0;
	for( uint16_t i = 0; i < 24; i++ ) {
		if( i > 11 ) print( "\e[38;5;0m" );
		printf( "\e[48;5;%um  %u \e[0m", i + 232, i + 232 );
		if( c++ == 11 ) { print( "\n" ); c = 0; }
	}

	// exit
	return 0;
}
