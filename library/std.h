/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	STD
	#define	STD

	// definitions, that are always nice to have
	#include	"stdint.h"
	#include	"stddef.h"
	#include	"stdarg.h"
	#include	"./macro.h"

	#define	EMPTY						0

	#define	TRUE						1
	#define	FALSE						0

	#define	LOCK						TRUE
	#define	UNLOCK						FALSE

	// listed alphabetically below

	#define	STD_COLOR_mask					0xFF000000
	#define	STD_COLOR_WHITE					0xFFFFFFFF
	#define	STD_COLOR_BLACK					0xFF000000
	#define	STD_COLOR_BLACK_light				0xFF080808
	#define	STD_COLOR_GREEN_light				0xFF10FF10
	#define	STD_COLOR_BLUE					0xFF003366
	#define	STD_COLOR_RED_light				0xFFFF1010

	#define	STD_FILE_MODE_mask				0b0000000111111111
	#define	STD_FILE_MODE_other_exec			0b0000000000000001
	#define	STD_FILE_MODE_other_write			0b0000000000000010
	#define	STD_FILE_MODE_other_read			0b0000000000000100
	#define	STD_FILE_MODE_group_exec			0b0000000000001000
	#define	STD_FILE_MODE_group_write			0b0000000000010000
	#define	STD_FILE_MODE_group_read			0b0000000000100000
	#define	STD_FILE_MODE_user_exec				0b0000000001000000
	#define	STD_FILE_MODE_user_write			0b0000000010000000
	#define	STD_FILE_MODE_user_read				0b0000000100000000
	#define	STD_FILE_MODE_sticky				0b0000001000000000
	#define	STD_FILE_MODE_default_directory			(STD_FILE_MODE_user_read | STD_FILE_MODE_user_write | STD_FILE_MODE_user_exec | STD_FILE_MODE_group_read | STD_FILE_MODE_group_write | STD_FILE_MODE_group_exec | STD_FILE_MODE_other_read | STD_FILE_MODE_other_exec)

	#define	STD_FILE_TYPE_fifo				0b00000001
	#define	STD_FILE_TYPE_character_device			0b00000010
	#define	STD_FILE_TYPE_directory				0b00000100
	#define	STD_FILE_TYPE_block_device			0b00001000
	#define	STD_FILE_TYPE_regular_file			0b00010000
	#define	STD_FILE_TYPE_symbolic_link			0b00100000
	#define	STD_FILE_TYPE_socket				0b01000000

	struct	STD_FILE_STRUCTURE {
		uint64_t	id;
		uint64_t	size_byte;
		uint8_t		type;
		uint16_t	mode;
		uint16_t	uid;
		uint16_t	guid;
	};

	#define	STD_IPC_SIZE_byte				40

	struct	STD_IPC_STRUCTURE {
		volatile uint64_t	ttl;
		int64_t		source;
		int64_t		target;
		uint8_t		data[ STD_IPC_SIZE_byte ];
	} __attribute__( (packed) );

	#define	STD_PAGE_byte					0x1000
	#define	STD_PAGE_mask					0xFFFFFFFFFFFFF000

	#define	STD_SHIFT_4					2
	#define	STD_SHIFT_8					3
	#define	STD_SHIFT_16					4
	#define	STD_SHIFT_32					5
	#define	STD_SHIFT_64					6
	#define	STD_SHIFT_PTR					STD_SHIFT_64
	#define	STD_SHIFT_128					7
	#define	STD_SHIFT_256					8
	#define	STD_SHIFT_512					9
	#define	STD_SHIFT_1024					10
	#define	STD_SHIFT_2048					11
	#define	STD_SHIFT_4096					12
	#define	STD_SHIFT_PAGE					STD_SHIFT_4096

	#define	STD_SYSCALL_EXIT				0x00
	#define	STD_SYSCALL_FRAMEBUFFER				0x01
	#define	STD_SYSCALL_MEMORY_ALLOC			0x02
	#define	STD_SYSCALL_MEMORY_RELEASE			0x03
	#define	STD_SYSCALL_UPTIME				0x04
	#define	STD_SYSCALL_LOG					0x05
	#define	STD_SYSCALL_THREAD				0x06
	#define	STD_SYSCALL_PID					0x07
	#define	STD_SYSCALL_EXEC				0x08
	#define	STD_SYSCALL_PID_CHECK				0x09
	#define	STD_SYSCALL_IPC_SEND				0x0A
	#define	STD_SYSCALL_IPC_RECEIVE				0x0B
	#define	STD_SYSCALL_MEMORY_SHARE			0x0C

	struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER {
		uint32_t	*base_address;
		uint16_t	width_pixel;
		uint16_t	height_pixel;
		uint64_t	pitch_byte;
		int64_t		owner_pid;
	};

	#define	STD_VIDEO_DEPTH_shift				2
	#define	STD_VIDEO_DEPTH_byte				4
	#define	STD_VIDEO_DEPTH_bit				32

	// stop process execution
	void std_exit( void );

	// returns properties of available framebuffer ()
	void std_framebuffer( struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER *framebuffer );

	// returns current miliseconds from system initialization
	uint64_t std_uptime( void );

	// returns process ID of executed function as thread
	int64_t std_thread( uintptr_t function, uint8_t *string, uint64_t length );

	// returns pid number of calling process
	int64_t std_pid( void );

	// returns N page sized area regarding of passsed Bytes
	uintptr_t std_memory_alloc( uint64_t byte );

	// returns ID of newly executed process
	int64_t std_exec( uint8_t *string, uint64_t length );

	// returns TRUE/FALSE, task exist?
	uint8_t std_pid_check( int64_t pid );

	// send data string to process with ID
	void std_ipc_send( int64_t pid, uint8_t *data );

	// fills data with message content and returns from which process ID it is
	int64_t std_ipc_receive( uint8_t *data );

	// connect source memory area with targets and inform about target pointer address
	uintptr_t std_memory_share( int64_t pid, uintptr_t address, uint64_t page );

	#ifdef	SOFTWARE
		// function definitions

		// requests syscall and returns nothing
		void std_syscall_empty( void );

		// initial function of every process
		extern int64_t _main( uint64_t argc, uint8_t *argv[] );

		// initialization of process environment
		void _entry( void ) {
			// sad hack :|
			__asm__ volatile( "testw $0x08, %sp\nje .+4\npushq $0x00" );

			// execute process flow
			int64_t result;	// initialize local variable
			__asm__ volatile( "call _main" : "=a" (result) );

			// execute leave out routine
			__asm__ volatile( "" :: "a" (STD_SYSCALL_EXIT) );
			std_syscall_empty();

			// SHOULD NOT HAPPEN... Hodor, You know what to do :D
			while( TRUE );
		}
	#endif

	//------------------------------------------------------------------------------
	// substitute of internal functions required by clang
	//------------------------------------------------------------------------------

	void memcpy( uint8_t *target, uint8_t *source, uint64_t length ) {
		// copy source content inside target
		for( uint64_t i = 0; i < length; i++) target[ i ] = source[ i ];
	}

	void memset( uint8_t *cache, uint8_t value, uint64_t length ) {
		// fill cache with definied value
		for( uint64_t i = 0; i < length; i++ )
			cache[ i ] = value;
	}

	//------------------------------------------------------------------------------
	// substitute of libc
	//------------------------------------------------------------------------------

	void *malloc( size_t byte );
	void *realloc( void *source, size_t byte );
	void free( void *source );
	double strtof( uint8_t *string, uint64_t length );
	uint64_t abs( int64_t i );
	double fmod( double x, double y );
	float sqrtf( float x );
	double minf( double first, double second );
	double maxf( double first, double second );
	void log( const char *string, ... );
#endif
