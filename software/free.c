/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct STD_STRUCTURE_STREAM_META stream_meta;

struct STD_STRUCTURE_SYSCALL_MEMORY memory;

void unit( uint8_t level ) {
	// default color
	print( "\e[38;5;245m" );

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

	// reset color
	print( "\e[0m" );
}

void ratio( uint64_t bytes ) {
	if( bytes >= 0x10000000000 ) { printf( "%4u ", bytes / 0x10000000000 ); unit( 4 ); return; }
	if( bytes >= 0x40000000 ) { printf( "%4u ", bytes / 0x40000000 ); unit( 3 ); return; }
	if( bytes >= 0x100000 ) { printf( "%5u ", bytes / 0x100000 ); unit( 2 ); return; }
	if( bytes >= 0x400 ) { printf( "%5u ", bytes / 0x400 ); unit( 1 ); return; }
	else printf( " %8s ", (uint8_t *) "Empty" );
}

void status( uint64_t total, uint64_t available ) {
	// width of status bar
	int16_t width = stream_meta.width - (58 + 7);
	if( width < 10 ) return;	// no enough space to show anything

	// calculate usage in percents
	uint8_t	percent = (uint8_t) (((double) (total - available) / (double) total) * 100.0f);
	uint8_t used = (uint8_t) (((double) (total - available) / (double) total) * (double) width);
	uint8_t free = ((double) available / (double) total) * (double) width;

	// by default status bar is green
	print( "\e[38;5;118m" );

	// if there is less than half or quartes space, change color
	if( available < total >> STD_SHIFT_2 ) print( "\e[38;5;226m" );
	if( available < total >> STD_SHIFT_4 ) print( "\e[38;5;196m" );

	// percents not always perfect ;)
	if( free + used < width ) free++;
	if( ! available ) { used++; free = 0; percent = 100; }

	// show usage
	while( used-- ) print( "|" );

	// and show left space in default color
	printf( "\e[38;5;239m" );
	while( free-- ) print( "|" );

	// by default status bar is green
	print( "\e[38;5;118m" );

	// if there is less than half or quartes space, change color
	if( available < total >> STD_SHIFT_2 ) print( "\e[38;5;226m" );
	if( available < total >> STD_SHIFT_4 ) print( "\e[38;5;196m" );

	// and at last %
	printf( " %3u%%", percent );
}

int64_t _main( uint64_t argc, uint8_t *argv[] ) {
	// retrieve stream meta data
	std_stream_get( (uint8_t *) &stream_meta, STD_STREAM_OUT );

	// retrieve properties of system memory
	std_memory( (struct STD_STRUCTURE_SYSCALL_MEMORY *) &memory );

	// show header
	print( "             Total    \e[38;5;241min\e[0m Use      Free      Page     Share\n" );

	// properties of available storages
	struct STD_STRUCTURE_STORAGE *storage = (struct STD_STRUCTURE_STORAGE *) std_storage();

	// preserve pointer base address
	uintptr_t storage_ptr = (uintptr_t) storage;

	// check longest storage name
	uint64_t name_limit = EMPTY;
	while( (++storage) -> type ) if( name_limit < storage -> name_limit ) name_limit = storage -> name_limit; if( name_limit < 6 ) name_limit = 6;	// "Memory" string length

	// internal memory
	printf( " %*s: ", name_limit, (uint8_t *) "Memory" ); ratio( memory.total ); ratio( memory.total - memory.available ); ratio( memory.available );  ratio( memory.paging );  ratio( memory.shared ); status( memory.total, memory.available ); print( "\n" );

	// storage by storage
	storage = (struct STD_STRUCTURE_STORAGE *) storage_ptr;
	do {
		// show its name
		printf( " \e[0m%*s: ", name_limit, storage -> name );

		// total size
		ratio( storage -> limit );
		// used
		ratio( storage -> limit - storage -> available );
		// free
		ratio( storage -> available );

		// N/A cells
		printf( "\e[38;5;239m%*s\e[0m ", 9, (uint8_t *) "---" );
		printf( "\e[38;5;239m%*s\e[0m ", 9, (uint8_t *) "---" );

		// visual usage representation
		status( storage -> limit, storage -> available );

		// move cursor to next line
		print( "\n");
	} while( (++storage) -> type );

	// release obtained storages properties
	std_memory_release( storage_ptr, MACRO_PAGE_ALIGN_UP( (uintptr_t) storage - storage_ptr ) >> STD_SHIFT_PAGE );

	// exit
	return 0;
}
