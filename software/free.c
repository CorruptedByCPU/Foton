/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct STD_STRUCTURE_STREAM_META stream_meta;

struct STD_STRUCTURE_SYSCALL_MEMORY memory;

void unit( uint8_t level ) {
	// select unit
	switch( level ) {
		case 1: { print( "KiB " ); break; }
		case 2: { print( "MiB " ); break; }
		case 3: { print( "GiB " ); break; }
		case 4: { print( "TiB " ); break; }
		default: 
			// undefinied
			print( "Err" );
	}
}

void ratio( uint64_t bytes ) {
	if( bytes >= 0x10000000000 ) { printf( "%4u ", bytes / 0x10000000000 ); unit( 4 ); return; }
	if( bytes >= 0x40000000 ) { printf( "%4u ", bytes / 0x40000000 ); unit( 3 ); return; }
	if( bytes >= 0x100000 ) { printf( "%5u ", bytes / 0x100000 ); unit( 2 ); return; }
	if( bytes >= 0x400 ) { printf( "%5u ", bytes / 0x400 ); unit( 1 ); return; }
}

void status( uint64_t total, uint64_t available ) {
	// width of status bar
	int16_t width = stream_meta.width - (58 + 4);
	if( width < 10 ) return;	// no enough space to show anything

	// calculate usage in percents
	uint8_t used = (uint8_t) (((double) (total - available) / (double) total) * (double) width);
	uint8_t free = ((double) available / (double) total) * (double) width;

	// by default status bar is green
	print( "\e[38;5;118m" );

	// if there is less than half or quartes space, change color
	if( available < total >> STD_SHIFT_2 ) print( "\e[38;5;226m" );
	if( available < total >> STD_SHIFT_4 ) print( "\e[38;5;196m" );

	// show usage
	while( used-- ) print( "|" );

	// and show left space in default color
	printf( "\e[38;5;239m" );
	while( free-- ) print( "|" );

	// and at last %
	printf( "%3u%%", (uint64_t) (((double) (total - available) / (double) total) * 100.0f) );
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// retrieve stream meta data
	std_stream_get( (uint8_t *) &stream_meta, STD_STREAM_OUT );

	// retrieve properties of system memory
	std_memory( (struct STD_STRUCTURE_SYSCALL_MEMORY *) &memory );

	// show header
	print( "             Total       Use      Free      Page     Share\n" );

	//----------------------------------------------------------------------

	// internal memory
	print( " Memory: " ); ratio( memory.total ); ratio( memory.total - memory.available ); ratio( memory.available );  ratio( memory.paging );  ratio( memory.shared ); status( memory.total, memory.available );

	//----------------------------------------------------------------------

	// to be done... all available storages

	// exit
	return 0;
}
