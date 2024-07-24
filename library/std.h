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

	#ifndef	LIB_VFS
		#include	"./vfs.h"
	#endif

	#define	EMPTY						0
	#define	INIT						EMPTY

	#define	TRUE						1
	#define	FALSE						0

	#define	LOCK						TRUE
	#define	UNLOCK						FALSE

	// listed alphabetically below

	#define	STD_ASCII_TERMINATOR				EMPTY
	#define	STD_ASCII_DIFFERENCE_0_A			0x07
	#define	STD_ASCII_BACKSPACE				0x08
	#define	STD_ASCII_TAB					0x09
	#define	STD_ASCII_NEW_LINE				0x0A
	#define	STD_ASCII_RETURN				0x0D
	#define	STD_ASCII_ESC					0x1B
	#define	STD_ASCII_SPACE					0x20
	#define	STD_ASCII_EXCLAMATION				0x21
	#define	STD_ASCII_QUOTATION				0x22
	#define	STD_ASCII_DOLLAR				0x24
	#define STD_ASCII_PERCENT				0x25
	#define	STD_ASCII_AMPERSAND				0x26
	#define	STD_ASCII_APOSTROPHE				0x27
	#define	STD_ASCII_ASTERISK				0x2A
	#define	STD_ASCII_PLUS					0x2B
	#define	STD_ASCII_COMMA					0x2C
	#define	STD_ASCII_MINUS					0x2D
	#define	STD_ASCII_DOT					0x2E
	#define	STD_ASCII_SLASH					0x2F
	#define	STD_ASCII_DIGIT_0				0x30
	#define	STD_ASCII_DIGIT_1				0x31
	#define	STD_ASCII_DIGIT_2				0x32
	#define	STD_ASCII_DIGIT_3				0x33
	#define	STD_ASCII_DIGIT_4				0x34
	#define	STD_ASCII_DIGIT_5				0x35
	#define	STD_ASCII_DIGIT_6				0x36
	#define	STD_ASCII_DIGIT_7				0x37
	#define	STD_ASCII_DIGIT_8				0x38
	#define	STD_ASCII_DIGIT_9				0x39
	#define STD_ASCII_COLON					0x3A
	#define	STD_ASCII_SEMICOLON				0x3B
	#define STD_ASCII_LOWER_THAN				0x3C
	#define	STD_ASCII_GREATER_THAN				0x3E
	#define	STD_ASCII_QUESTION_MARK				0x3F
	#define	STD_ASCII_LETTER_A				0x41
	#define	STD_ASCII_LETTER_F				0x46
	#define	STD_ASCII_LETTER_N				0x4E
	#define	STD_ASCII_LETTER_T				0x54
	#define	STD_ASCII_LETTER_Z				0x5A
	#define	STD_ASCII_BRACKET_SQUARE_OPEN			0x5B
	#define	STD_ASCII_BACKSLASH				0x5C
	#define	STD_ASCII_BRACKET_SQUARE_CLOSE			0x5D
	#define	STD_ASCII_CARET					0x5E
	#define	STD_ASCII_GRAVE					0x60
	#define	STD_ASCII_LETTER_a				0x61
	#define	STD_ASCII_LETTER_b				0x62
	#define	STD_ASCII_LETTER_c				0x63
	#define	STD_ASCII_LETTER_d				0x64
	#define	STD_ASCII_LETTER_f				0x66
	#define	STD_ASCII_LETTER_h				0x68
	#define	STD_ASCII_LETTER_i				0x69
	#define	STD_ASCII_LETTER_l				0x6C
	#define	STD_ASCII_LETTER_n				0x6E
	#define	STD_ASCII_LETTER_q				0x71
	#define	STD_ASCII_LETTER_r				0x72
	#define	STD_ASCII_LETTER_s				0x73
	#define	STD_ASCII_LETTER_t				0x74
	#define	STD_ASCII_LETTER_u				0x75
	#define	STD_ASCII_LETTER_x				0x78
	#define	STD_ASCII_LETTER_z				0x7A
	#define	STD_ASCII_BRACLET_OPEN				0x7B
	#define	STD_ASCII_STREAM				0x7C
	#define	STD_ASCII_BRACLET_CLOSE				0x7D
	#define	STD_ASCII_TILDE					0x7E
	#define	STD_ASCII_DELETE				0x7F

	#define	STD_COLOR_mask					0xFF000000
	#define	STD_COLOR_BLACK					0xFF000000
	#define	STD_COLOR_DARK					0xFF272727
	#define	STD_COLOR_RED					0xFF800000
	#define	STD_COLOR_GREEN					0xFF008000
	#define	STD_COLOR_BROWN					0xFF808000
	#define	STD_COLOR_BLUE					0xFF000080
	#define	STD_COLOR_MAGENTA				0xFF800080
	#define	STD_COLOR_CYAN					0xFF008080
	#define	STD_COLOR_GRAY_LIGHT				0xFFC0C0C0
	#define	STD_COLOR_GRAY					0xFF808080
	#define	STD_COLOR_REG_LIGHT				0xFFFF0000
	#define	STD_COLOR_GREEN_LIGHT				0xFFFFFF00
	#define	STD_COLOR_YELLOW				0xFF00FF00
	#define	STD_COLOR_BLUE_LIGHT				0xFF0000FF
	#define	STD_COLOR_MAGENTA_LIGHT				0xFFFF00FF
	#define	STD_COLOR_CYAN_LIGHT				0xFF00FFFF
	#define	STD_COLOR_WHITE					0xFFFFFFFF

	#define	STD_ERROR_file_not_found			-1
	#define	STD_ERROR_memory_low				-2
	#define	STD_ERROR_file_unknown				-3
	#define	STD_ERROR_file_not_executable			-4
	#define	STD_ERROR_syntax_error				-5	// provided values or structure is invalid
	#define	STD_ERROR_overflow				-6
	#define	STD_ERROR_file_dependence			-7
	#define	STD_ERROR_limit					-8
	#define	STD_ERROR_locked				-9
	#define	STD_ERROR_unavailable				-10

	#define	STD_FILE_TYPE_file				0b00000001
	#define	STD_FILE_TYPE_directory				0b00000010
	#define	STD_FILE_TYPE_link				0b00000100

	// #define	STD_FILE_MODE_reserved				0b00000001
	// #define	STD_FILE_MODE_read				0b00000010
	// #define	STD_FILE_MODE_write				0b00000100
	// #define	STD_FILE_MODE_append				0b00001000

	#define	STD_FILE_NAME_limit				LIB_VFS_NAME_limit

	#define	STD_FILE_TOUCH_file				STD_FILE_TYPE_file
	#define	STD_FILE_TOUCH_directory			STD_FILE_TYPE_directory

	struct	STD_FILE_STRUCTURE {
		int64_t		socket;
		uint64_t	byte;
		uint64_t	seek;
		uint16_t	name_length;
		uint8_t		name[ STD_FILE_NAME_limit ];
	};

	#define	STD_IPC_SIZE_byte				40

	#define	STD_IPC_TYPE_keyboard				0x00
	#define	STD_IPC_TYPE_mouse				0x01
	#define	STD_IPC_TYPE_event				0xFF

	#define	STD_IPC_MOUSE_BUTTON_left			0b00000001
	#define	STD_IPC_MOUSE_BUTTON_right			0b00000010
	#define	STD_IPC_MOUSE_BUTTON_middle			0b00000100
	#define	STD_IPC_MOUSE_BUTTON_release			0b10000000

	struct	STD_IPC_STRUCTURE {
		volatile uint64_t	ttl;
		int64_t		source;
		int64_t		target;
		uint8_t		data[ STD_IPC_SIZE_byte ];	// first Byte of data defines TYPE
	} __attribute__( (packed) );

	struct	STD_IPC_STRUCTURE_DEFAULT {
		uint8_t		type;
	} __attribute__( (packed) );

	struct	STD_IPC_STRUCTURE_KEYBOARD {
		struct STD_IPC_STRUCTURE_DEFAULT	ipc;
		uint16_t	key;
	} __attribute__( (packed) );

	struct	STD_IPC_STRUCTURE_MOUSE {
		struct STD_IPC_STRUCTURE_DEFAULT	ipc;
		uint8_t		button;
		int16_t		scroll;
	} __attribute__( (packed) );

	struct STD_IPC_STRUCTURE_WINDOW {
		struct STD_IPC_STRUCTURE_DEFAULT	ipc;
		int16_t		x;
		int16_t		y;
		uint16_t	width;
		uint16_t	height;
	} __attribute__( (packed) );

	struct STD_IPC_STRUCTURE_WINDOW_DESCRIPTOR {
		struct STD_IPC_STRUCTURE_DEFAULT	ipc;
		uintptr_t	descriptor;
	} __attribute__( (packed) );

	#define	STD_KEY_BACKSPACE				0x0008
	#define	STD_KEY_TAB					0x0009
	#define	STD_KEY_LINE					0x000A
	#define	STD_KEY_ENTER					0x000D
	#define	STD_KEY_ESC					0x001B
	#define	STD_KEY_CTRL_LEFT				0x001D
	#define	STD_KEY_SPACE					0x0020
	#define	STD_KEY_RELEASE					0x0080
	#define	STD_KEY_SHIFT_LEFT				0xE02A
	#define	STD_KEY_SHIFT_RIGHT				0xE036
	#define	STD_KEY_NUMLOCK_MULTIPLY			0xE037
	#define	STD_KEY_ALT_LEFT				0xE038
	#define	STD_KEY_CAPSLOCK				0xE03A
	#define	STD_KEY_F1					0xE03B
	#define	STD_KEY_F2					0xE03C
	#define	STD_KEY_F3					0xE03D
	#define	STD_KEY_F4					0xE03E
	#define	STD_KEY_F5					0xE03F
	#define	STD_KEY_F6					0xE040
	#define	STD_KEY_F7					0xE041
	#define	STD_KEY_F8					0xE042
	#define	STD_KEY_F9					0xE043
	#define	STD_KEY_F10					0xE044
	#define	STD_KEY_NUMLOCK					0xE045
	#define	STD_KEY_SCROLL_LOCK				0xE046
	#define	STD_KEY_NUMLOCK_MINUS				0xE04A
	#define	STD_KEY_NUMLOCK_4				0xE14B
	#define	STD_KEY_NUMLOCK_5				0xE04C
	#define	STD_KEY_NUMLOCK_6				0xE14D
	#define	STD_KEY_NUMLOCK_PLUS				0xE04E
	#define	STD_KEY_NUMLOCK_7				0xE047
	#define	STD_KEY_NUMLOCK_8				0xE048
	#define	STD_KEY_NUMLOCK_9				0xE049
	#define	STD_KEY_NUMLOCK_1				0xE14F
	#define	STD_KEY_NUMLOCK_2				0xE150
	#define	STD_KEY_NUMLOCK_3				0xE151
	#define	STD_KEY_NUMLOCK_0				0xE152
	#define	STD_KEY_NUMLOCK_DOT				0xE153
	#define	STD_KEY_F11					0xE057
	#define	STD_KEY_F12					0xE158
	#define	STD_KEY_CTRL_RIGHT				0xE01D
	#define	STD_KEY_NUMLOCK_DIVIDE				0xE035
	#define	STD_KEY_ALT_RIGHT				0xE038
	#define	STD_KEY_HOME					0xE047
	#define	STD_KEY_ARROW_UP				0xE048
	#define	STD_KEY_PAGE_UP					0xE049
	#define	STD_KEY_ARROW_LEFT				0xE04B
	#define	STD_KEY_ARROW_RIGHT				0xE04D
	#define	STD_KEY_END					0xE04F
	#define	STD_KEY_ARROW_DOWN				0xE050
	#define	STD_KEY_PAGE_DOWN				0xE051
	#define	STD_KEY_INSERT					0xE052
	#define	STD_KEY_DELETE					0xE053
	#define	STD_KEY_MENU					0xE05B
	#define	STD_KEY_SUBMENU					0xE05D

	struct	STD_SYSCALL_STRUCTURE_MEMORY {
		uint64_t	total;
		uint64_t	available;
		uint64_t	paging;
		uint64_t	shared;
	};

	#define	STD_MAX_unsigned				-1

	#define	STD_MOUSE_BUTTON_left				0b00000001
	#define	STD_MOUSE_BUTTON_right				0b00000010
	#define	STD_MOUSE_BUTTON_middle				0b00000100

	struct STD_SYSCALL_STRUCTURE_MOUSE {
		uint16_t	x;
		uint16_t	y;
		uint8_t		status;
	};

	#define	STD_MOVE_BYTE					8
	#define	STD_MOVE_WORD					16
	#define	STD_MOVE_DWORD					32

	#define	STD_NETWORK_PROTOCOL_arp			0x00
	#define	STD_NETWORK_PROTOCOL_icmp			0x01
	#define	STD_NETWORK_PROTOCOL_udp			0x02
	#define	STD_NETWORK_PROTOCOL_tcp			0x03

	struct STD_NETWORK_STRUCTURE_DATA {
		uint8_t		*data;
		uint64_t	length;
	};

	struct STD_NETWORK_STRUCTURE_INTERFACE {
		uint8_t		ethernet_address[ 6 ];
		uint32_t	ipv4_address;
		uint32_t	ipv4_mask;
		uint32_t	ipv4_broadcast;
		uint32_t	ipv4_gateway;
		// statistics: read only
		uint64_t	rx_frame;
		uint64_t	rx_byte;
		uint64_t	tx_frame;
		uint64_t	tx_byte;
	};

	#define	STD_PAGE_byte					0x1000
	#define	STD_PAGE_mask					0xFFFFFFFFFFFFF000

	#define	STD_PTR_byte					0x08

	#define	STD_SHIFT_2					1
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
	#define	STD_SHIFT_65536					16

	#define	STD_SIZE_BYTE_byte				1
	#define	STD_SIZE_BYTE_bit				8
	#define	STD_SIZE_WORD_byte				2
	#define	STD_SIZE_WORD_bit				16
	#define	STD_SIZE_DWORD_byte				4
	#define	STD_SIZE_DWORD_bit				32
	#define	STD_SIZE_QWORD_byte				8
	#define	STD_SIZE_QWORD_bit				64

	#define	STD_STREAM_SIZE_page				1	// less or equal to 16, limited by struct KERNEL_STREAM_STRUCTURE

	#define	STD_STREAM_FLOW_out_to_parent_in		0b00000001
	#define	STD_STREAM_FLOW_out_to_in			0b00000010

	#define	STD_STREAM_OUT					0x00
	#define	STD_STREAM_IN					0x01

	#define	STD_STREAM_META_limit				8

	struct	STD_STREAM_STRUCTURE_META {
		uint16_t	x;
		uint16_t	y;
		uint16_t	width;
		uint16_t	height;
	} __attribute__( (packed) );

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
	#define	STD_SYSCALL_MOUSE				0x0D
	#define	STD_SYSCALL_FRAMEBUFFER_CHANGE			0x0E
	#define	STD_SYSCALL_IPC_RECEIVE_BY_PID			0x0F
	#define	STD_SYSCALL_STREAM_OUT				0x10
	#define	STD_SYSCALL_STREAM_IN				0x11
	#define	STD_SYSCALL_KEYBOARD				0x12
	#define	STD_SYSCALL_STREAM_SET				0x13
	#define	STD_SYSCALL_STREAM_GET				0x14
	#define	STD_SYSCALL_MEMORY				0x15
	#define	STD_SYSCALL_SLEEP				0x16
	#define	STD_SYSCALL_FILE_OPEN				0x17
	#define	STD_SYSCALL_FILE_CLOSE				0x18
	#define	STD_SYSCALL_CD					0x19
	#define	STD_SYSCALL_IPC_RECEIVE_BY_TYPE			0x1A
	#define	STD_SYSCALL_MICROTIME				0x1B
	#define	STD_SYSCALL_TIME				0x1C
	#define	STD_SYSCALL_FILE_READ				0x1D
	#define	STD_SYSCALL_FILE				0x1E
	#define	STD_SYSCALL_FILE_WRITE				0x1F
	#define	STD_SYSCALL_FILE_TOUCH				0x20
	#define	STD_SYSCALL_TASK				0x21
	#define	STD_SYSCALL_KILL				0x22
	#define	STD_SYSCALL_NETWORK_INTERFACE			0x23
	#define	STD_SYSCALL_NETWORK_OPEN			0x24
	#define	STD_SYSCALL_NETWORK_SEND			0x25
	#define	STD_SYSCALL_NETWORK_INTERFACE_SET		0x26
	#define	STD_SYSCALL_NETWORK_RECEIVE			0x27

	struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER {
		uint32_t	*base_address;
		uint16_t	width_pixel;
		uint16_t	height_pixel;
		uint64_t	pitch_byte;
		int64_t		pid;
	};

	#define	STD_TASK_FLAG_active				0b0000000000000001
	#define	STD_TASK_FLAG_exec				0b0000000000000010
	#define	STD_TASK_FLAG_close				0b0000000000000100
	#define	STD_TASK_FLAG_module				0b0000000000001000
	#define	STD_TASK_FLAG_thread				0b0000000000010000
	#define	STD_TASK_FLAG_sleep				0b0000000000100000
	#define	STD_TASK_FLAG_init				0b0100000000000000
	#define	STD_TASK_FLAG_secured				0b1000000000000000

	struct STD_SYSCALL_STRUCTURE_TASK {
		int64_t		pid;
		uint64_t	page;
		uint64_t	stack;
		uint8_t		flags;
		uint64_t	time;
		uint8_t		name_length;
		uint8_t		name[ 255 ];
	};

	#define	STD_VIDEO_DEPTH_shift				2
	#define	STD_VIDEO_DEPTH_byte				4
	#define	STD_VIDEO_DEPTH_bit				32

	#define	STD_WINDOW_FLAG_flush		0b0000000000000001
	#define	STD_WINDOW_FLAG_visible		0b0000000000000010
	#define	STD_WINDOW_FLAG_fixed_xy	0b0000000000000100
	#define	STD_WINDOW_FLAG_fixed_z		0b0000000000001000
	#define	STD_WINDOW_FLAG_release		0b0000000000010000	// window marked as ready to be removed
	#define	STD_WINDOW_FLAG_name		0b0000000000100000
	#define	STD_WINDOW_FLAG_minimize	0b0000000001000000
	#define	STD_WINDOW_FLAG_unstable	0b0000000010000000	// hide window on any mouse button press
	#define	STD_WINDOW_FLAG_resizable	0b0000000100000000
	#define	STD_WINDOW_FLAG_properties	0b0000001000000000	// Window Manager proposed new window properties
	#define	STD_WINDOW_FLAG_maximize	0b0000010000000000
	#define	STD_WINDOW_FLAG_taskbar		0b0100000000000000
	#define	STD_WINDOW_FLAG_cursor		0b1000000000000000

	#define	STD_WINDOW_REQUEST_create	0b00000001
	#define	STD_WINDOW_REQUEST_active	0b00000010

	#define	STD_WINDOW_ANSWER_create	0b10000000 | STD_WINDOW_REQUEST_create
	#define	STD_WINDOW_ANSWER_active	0b10000000 | STD_WINDOW_REQUEST_active

	struct	STD_WINDOW_STRUCTURE_DESCRIPTOR {
		uint16_t	flags;
		// pointer position inside window
		uint16_t	x;
		uint16_t	y;
		// proposed window properties
		int16_t		new_x;
		int16_t		new_y;
		uint16_t	new_width;
		uint16_t	new_height;
		// window name, it will appear at header and taskbar
		uint8_t		name_length;
		uint8_t		name[ 64 ];
	} __attribute__( (aligned( STD_PAGE_byte )) );

	#define	STD_WORD_mask			0x000000000000FFFF

	// returns properties of available framebuffer ()
	void std_framebuffer( struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER *framebuffer );

	// returns current miliseconds from system initialization
	uint64_t std_uptime( void );

	// returns process ID of executed function as thread
	int64_t std_thread( uintptr_t function, uint8_t *string, uint64_t length );

	// returns pid number of calling process
	int64_t std_pid( void );

	// returns N page sized area
	uintptr_t std_memory_alloc( uint64_t page );

	// releases memory area of N pages
	void std_memory_release( uintptr_t target, uint64_t page );

	// returns ID of newly executed process
	int64_t std_exec( uint8_t *string, uint64_t length, uint8_t stream_flow );

	// returns TRUE/FALSE, task exist?
	uint8_t std_pid_check( int64_t pid );

	// send data string to process with ID
	void std_ipc_send( int64_t pid, uint8_t *data );

	// fills data with message content and returns from which process ID it is
	int64_t std_ipc_receive( uint8_t *data );

	// connect source memory area with targets and inform about target pointer address
	uintptr_t std_memory_share( int64_t pid, uintptr_t address, uint64_t page );

	// returns properties of mouse pointing device
	void std_mouse( struct STD_SYSCALL_STRUCTURE_MOUSE *mouse );

	// modify properties of kernels framebuffer
	void std_framebuffer_change( struct STD_SYSCALL_STRUCTURE_FRAMEBUFFER *framebuffer );

	// check for message from process of ID
	uint8_t std_ipc_receive_by_pid( uint8_t *data, int64_t pid );

	// send string to default output, if not possible FALSE returned
	uint8_t std_stream_out( uint8_t *string, uint64_t length );

	// retrieve data from stream, if exist
	uint64_t std_stream_in( uint8_t *target );

	// get key from kernel buffer
	uint16_t std_keyboard( void );

	// update stream meta data
	void std_stream_set( uint8_t *meta, uint8_t stream_type );

	// retrieves meta data of stream, or TRUE/FALSE regarding if it was up to date
	uint8_t std_stream_get( uint8_t *target, uint8_t stream_type );

	// returns properties of system memory
	void std_memory( struct STD_SYSCALL_STRUCTURE_MEMORY *memory );

	// releases AP rest of time for N units, returns N left units if sleep is broken
	uint64_t std_sleep( uint64_t units );	// 1 unit ~ 1/1024 of second

	// print character
	void putc( uint8_t character );

	// change root directory of current process
	uint8_t std_cd( uint8_t *path, uint64_t path_length );

	// check for messages of definied type, return PID of source
	int64_t std_ipc_receive_by_type( uint8_t *data, uint8_t type );

	// returns passed microtime since boot
	uint64_t std_microtime( void );

	// returns information about current date and time in format: 0x00wwyymmddHHMMSS
	uint64_t std_time( void );

	// open connection to file
	int64_t std_file_open( uint8_t *path, uint64_t path_length );

	// close connection to file
	void std_file_close( int64_t socket );

	// retrieve file properties
	void std_file( struct STD_FILE_STRUCTURE *file );

	// read file content into memory
	void std_file_read( struct STD_FILE_STRUCTURE *file, uint8_t *target, uint64_t byte );

	// write content of memory into file
	void std_file_write( struct STD_FILE_STRUCTURE *file, uint8_t *source, uint64_t byte );

	// create empty file of definied type
	int64_t std_file_touch( uint8_t *path, uint8_t type );

	// returns structure of currently running tasks
	uintptr_t std_task( void );

	// close process with selected PID
	void std_kill( int64_t pid );

	// returns properties of interface
	void std_network_interface( struct STD_NETWORK_STRUCTURE_INTERFACE *interface );

	// open network connection
	int64_t std_network_open( uint8_t protocol, uint32_t ipv4_target, uint16_t port_target, uint16_t port_local );

	// send data to socket
	int64_t std_network_send( int64_t socket, uint8_t *data, uint64_t length );

	// modify properties of interface
	void std_network_interface_set( struct STD_NETWORK_STRUCTURE_INTERFACE *interface );

	// receive data from socket
	void std_network_receive( int64_t socket, struct STD_NETWORK_STRUCTURE_DATA *data );

	#ifdef	SOFTWARE
		struct	STD_STRUCTURE_ENTRY {
			uint64_t	length;
			uint8_t		*string;
		} __attribute__( (packed) );

		// function definitions

		// requests syscall and returns nothing
		void std_syscall_empty( void );

		// initial function of every process
		extern int64_t _main( uint64_t argc, uint8_t *argv[] );

		// initialization of process environment
		void _entry( struct STD_STRUCTURE_ENTRY entry ) {
			// sad hack :|
			__asm__ volatile( "testw $0x08, %sp\nje .+4\npushq $0x00" );

			// remove "white" characters from beginning and end of string
			entry.length = lib_string_trim( entry.string, entry.length );

			// amount of args inside string
			uint64_t argc = 1;	// command itself is always an argument
			for( uint64_t i = 0; i < entry.length; i++ ) if( entry.string[ i ] == STD_ASCII_SPACE ) { argc++; for( ; i < entry.length; i++ ) if( entry.string[ i ] == STD_ASCII_SPACE ) break; }

			// allocate memory for argv vectors
			uint8_t *argv[ argc ];

			// insert pointers to every argument inside string
			uint64_t arg = 0;
			for( uint64_t i = 0; i < entry.length; i++ ) {
				// arg?
				if( entry.string[ i ] != STD_ASCII_SPACE ) {
					// save pointer to argument
					argv[ arg++ ] = (uint8_t *) &entry.string[ i ];

					// search for next arg
					for( ; i < entry.length; i++ ) if( entry.string[ i ] == STD_ASCII_SPACE ) { entry.string[ i ] = STD_ASCII_TERMINATOR; break; }
				}
			}

			// execute process flow
			int64_t result;	// initialize local variable
			__asm__ volatile( "call _main" : "=a" (result) : "D" (argc), "S" (argv) );

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

	typedef struct STD_FILE_STRUCTURE FILE;

	void *malloc( size_t byte );
	void *realloc( void *source, size_t byte );
	void *calloc( size_t byte );
	void free( void *source );
	double strtof( uint8_t *string, uint64_t length );
	uint64_t abs( int64_t i );
	double fmod( double x, double y );
	float sqrtf( float x );
	double minf( double first, double second );
	double maxf( double first, double second );
	void log( const char *string, ... );
	void print( const char *string );
	void printf( const char *string, ... );
	void sprintf( const char *string, ... );
	uint64_t pow( uint64_t base, uint64_t exponent );
	uint16_t getkey( void );
	void exit( void );
	FILE *fopen( uint8_t *path );
	void fclose( FILE *file );
	void fread( FILE *file, uint8_t *cache, uint64_t byte );
	void fwrite( FILE *file, uint8_t *cache, uint64_t byte );
	FILE *touch( uint8_t *path, uint8_t type );
	void sleep( uint64_t t );
#endif
