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

	#ifndef	LIB_STRING
		#include	"./string.h"
	#endif

	#define	EMPTY						0
	#define	INIT						EMPTY

	#define	FALSE						0
	#define	TRUE						1

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

	#define	STD_BIT_CONTROL_DWORD_bit			31	// or 0b00011111
	#define	STD_BIT_CONTROL_WORD_bit			15	// or 0b00001111

	#define	STD_COLOR_mask					0xFF000000
	#define	STD_COLOR_BLACK					0xFF000000
	#define	STD_COLOR_DARK					0xFF272727
	#define	STD_COLOR_RED					0xFF800000
	#define	STD_COLOR_GREEN					0xFF008000
	#define	STD_COLOR_BROWN					0xFF808000
	#define	STD_COLOR_BLUE					0xFF000080
	#define	STD_COLOR_MAGENTA				0xFF800080
	#define	STD_COLOR_CYAN					0xFF008080
	#define	STD_COLOR_GRAY_LIGHT				0xFF808080
	#define	STD_COLOR_GRAY					0xFF404040
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

	#define	STD_FILE_TYPE_file				0x01
	#define	STD_FILE_TYPE_directory				0x02
	#define	STD_FILE_TYPE_link				0x04

	#define	STD_FILE_MODE_modify				0x01

	#define	STD_FILE_NAME_limit				LIB_VFS_NAME_limit

	struct	STD_STRUCTURE_FILE {
		uint64_t	socket;
		uint64_t	byte;
		uint64_t	seek;
		uint8_t		type;
	};

	struct	STD_STRUCTURE_DIR {
		uint64_t	limit;
		uint8_t		type;
		uint8_t		name_limit;
		uint8_t		name[ LIB_VFS_NAME_limit + 1 ];
	};

	#define	STD_IPC_SIZE_byte				40

	#define	STD_IPC_TYPE_default				0x00
	#define	STD_IPC_TYPE_keyboard				0x01
	#define	STD_IPC_TYPE_mouse				0x02

	#define	STD_IPC_MOUSE_BUTTON_left			0x01
	#define	STD_IPC_MOUSE_BUTTON_right			0x02
	#define	STD_IPC_MOUSE_BUTTON_middle			0x04
	#define	STD_IPC_MOUSE_BUTTON_release			0x80

	#define	STD_IPC_WINDOW_create				0x00
	#define	STD_IPC_WINDOW_list				0x01

	struct	STD_STRUCTURE_IPC {
		volatile uint64_t	ttl;
		uint64_t		source;
		uint64_t		target;
		uint8_t			data[ STD_IPC_SIZE_byte ];	// first Byte of data defines TYPE
	} __attribute__( (packed) );

	struct	STD_STRUCTURE_IPC_DEFAULT {
		uint8_t		type;
	} __attribute__( (packed) );

	struct	STD_STRUCTURE_IPC_KEYBOARD {
		struct STD_STRUCTURE_IPC_DEFAULT	ipc;
		uint16_t				key;
	} __attribute__( (packed) );

	struct	STD_STRUCTURE_IPC_MOUSE {
		struct STD_STRUCTURE_IPC_DEFAULT	ipc;
		uint8_t					button;
		int8_t					scroll;	// deprecated, remove if no one else is using (todo)
	} __attribute__( (packed) );

	struct STD_STRUCTURE_IPC_WINDOW {
		struct STD_STRUCTURE_IPC_DEFAULT	ipc;
		uint8_t					properties;
	} __attribute__( (packed) );

	struct STD_STRUCTURE_IPC_WINDOW_CREATE {
		struct STD_STRUCTURE_IPC_DEFAULT	ipc;
		uint8_t					properties;
		int16_t					x;
		int16_t					y;
		uint16_t				width;
		uint16_t				height;
	} __attribute__( (packed) );

	struct STD_STRUCTURE_IPC_WINDOW_DESCRIPTOR {
		struct STD_STRUCTURE_IPC_DEFAULT	ipc;
		uintptr_t				descriptor;
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

	struct	STD_STRUCTURE_KEYBOARD_STATE {
		uint8_t	semaphore_alt_left;
		uint8_t	semaphore_ctrl_left;
		uint8_t	semaphore_shift;
	};

	struct	STD_STRUCTURE_SYSCALL_MEMORY {
		uint64_t	total;
		uint64_t	available;
		uint64_t	paging;
		uint64_t	shared;
	};

	#define	STD_MAX_unsigned				-1

	#define	STD_MOUSE_BUTTON_left				0x01
	#define	STD_MOUSE_BUTTON_right				0x02
	#define	STD_MOUSE_BUTTON_middle				0x04

	struct	STD_STRUCTURE_MOUSE_STATE {
		uint8_t	semaphore_left;
		uint8_t	semaphore_right;
		uint8_t	semaphore_middle;
	};

	struct STD_STRUCTURE_MOUSE_SYSCALL {
		uint16_t	x;
		uint16_t	y;
		int16_t		z;
		uint8_t		status;
	};

	#define	STD_MOVE_BYTE_half				4
	#define	STD_MOVE_BYTE					8
	#define	STD_MOVE_WORD					16
	#define	STD_MOVE_DWORD					32

	#define	STD_NETWORK_PROTOCOL_arp			0x00
	#define	STD_NETWORK_PROTOCOL_icmp			0x01
	#define	STD_NETWORK_PROTOCOL_udp			0x02
	#define	STD_NETWORK_PROTOCOL_tcp			0x03

	struct STD_STRUCTURE_NETWORK_DATA {
		uint8_t		*data;
		uint64_t	length;
	};

	struct STD_STRUCTURE_NETWORK_INTERFACE {
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

	#define	STD_NUMBER_SYSTEM_decimal			10
	#define	STD_NUMBER_SYSTEM_hexadecimal			16

	#define	STD_PAGE_page					1
	#define	STD_PAGE_byte					0x1000
	#define	STD_PAGE_mask					0xFFFFFFFFFFFFF000

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
	#define	STD_SHIFT_32768					15
	#define	STD_SHIFT_65536					16

	#define	STD_SIZE_BYTE_byte				1
	#define	STD_SIZE_BYTE_bit				8
	#define	STD_SIZE_WORD_byte				2
	#define	STD_SIZE_WORD_bit				16
	#define	STD_SIZE_DWORD_byte				4
	#define	STD_SIZE_DWORD_bit				32
	#define	STD_SIZE_QWORD_byte				8
	#define	STD_SIZE_QWORD_sign				((uint64_t) 1 << 63)
	#define	STD_SIZE_QWORD_bit				64
	#define	STD_SIZE_PTR_byte				STD_SIZE_QWORD_byte

	#define	STD_STORAGE_NAME_length				31				

	#define	STD_STORAGE_TYPE_memory				0x01
	#define	STD_STORAGE_TYPE_disk				0x02

	struct	STD_STRUCTURE_STORAGE {
		uint64_t 	id;
		uint64_t	limit;
		uint64_t	available;
		uint8_t		type;
		uint8_t		name_limit;
		uint8_t		name[ LIB_VFS_NAME_limit + 1 ];
	};

	#define	STD_STREAM_SIZE_page				1	// less or equal to 16, limited by struct KERNEL_STRUCTURE_STREAM

	#define	STD_STREAM_FLOW_out_to_parent_in		0x01
	#define	STD_STREAM_FLOW_out_to_in			0x02

	#define	STD_STREAM_OUT					0x00
	#define	STD_STREAM_IN					0x01

	#define	STD_STREAM_META_limit				8

	struct	STD_STRUCTURE_STREAM_META {
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
	#define	STD_SYSCALL_MEMORY_MOVE				0x16
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
	#define	STD_SYSCALL_STORAGE				0x28
	#define	STD_SYSCALL_STORAGE_SELECT			0x29
	#define	STD_SYSCALL_DIR					0x2A
	#define	STD_SYSCALL_STORAGE_ID				0x2B
	#define	STD_SYSCALL_MEMORY_PURGE			0x2C

	struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER {
		uint32_t	*base_address;
		uint16_t	width_pixel;
		uint16_t	height_pixel;
		uint64_t	pitch_byte;
		uint64_t	pid;
	};

	#define	STD_TASK_FLAG_active				0x0001	// 0b0000000000000001
	#define	STD_TASK_FLAG_exec				0x0002	// 0b0000000000000010
	#define	STD_TASK_FLAG_close				0x0004	// 0b0000000000000100
	#define	STD_TASK_FLAG_module				0x0008	// 0b0000000000001000
	#define	STD_TASK_FLAG_thread				0x0010	// 0b0000000000010000
	#define	STD_TASK_FLAG_sleep				0x0020	// 0b0000000000100000
	#define	STD_TASK_FLAG_init				0x4000	// 0b0100000000000000
	#define	STD_TASK_FLAG_secured				0x8000	// 0b1000000000000000

	struct STD_STRUCTURE_SYSCALL_TASK {
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

	#define	STD_MASK_byte_half		0x000000000000000F
	#define	STD_MASK_byte			0x00000000000000FF
	#define	STD_MASK_word			0x000000000000FFFF
	#define	STD_MASK_dword			0x00000000FFFFFFFF

	// returns properties of available framebuffer
	void std_framebuffer( struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER *framebuffer );

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
	int64_t std_exec( uint8_t *string, uint64_t length, uint8_t stream_flow, uint8_t detach );

	// returns TRUE/FALSE, task exist?
	uint8_t std_pid_exist( int64_t pid );

	// send data string to process with ID
	void std_ipc_send( int64_t pid, uint8_t *data );

	// fills data with message content and returns from which process ID it is
	int64_t std_ipc_receive( uint8_t *data );

	// connect source memory area with targets and inform about target pointer address
	uintptr_t std_memory_share( uint64_t pid, uintptr_t address, uint64_t page, uint8_t write );

	uintptr_t std_memory_move( uint64_t pid, uintptr_t address, uint64_t page );

	// returns properties of mouse pointing device
	void std_mouse( struct STD_STRUCTURE_MOUSE_SYSCALL *mouse );

	// modify properties of kernels framebuffer
	void std_framebuffer_change( struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER *framebuffer );

	// release memory marked as EXTERNAL
	void std_memory_purge( void );

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
	void std_memory( struct STD_STRUCTURE_SYSCALL_MEMORY *memory );

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
	int64_t std_file_open( uint8_t *path, uint64_t path_length, uint8_t mode );

	// close connection to file
	void std_file_close( int64_t socket );

	// retrieve file properties
	void std_file( struct STD_STRUCTURE_FILE *file );

	// read file content into memory
	void std_file_read( uint64_t socket_id, uint8_t *target, uint64_t seek, uint64_t byte );

	// write content of memory into file
	void std_file_write( struct STD_STRUCTURE_FILE *file, uint8_t *source, uint64_t byte );

	// create empty file of definied type
	int64_t std_file_touch( uint8_t *path, uint8_t type );

	// returns structure of currently running tasks
	uintptr_t std_task( void );

	// close process with selected PID
	void std_kill( int64_t pid );

	// returns properties of interface
	void std_network_interface( struct STD_STRUCTURE_NETWORK_INTERFACE *interface );

	// open network connection
	int64_t std_network_open( uint8_t protocol, uint32_t ipv4_target, uint16_t port_target, uint16_t port_local );

	// send data to socket
	int64_t std_network_send( int64_t socket, uint8_t *data, uint64_t length );

	// modify properties of interface
	void std_network_interface_set( struct STD_STRUCTURE_NETWORK_INTERFACE *interface );

	// receive data from socket
	void std_network_receive( int64_t socket, struct STD_STRUCTURE_NETWORK_DATA *data );

	// returns list of available storages
	uint64_t std_storage( void );

	// change main storage
	uint8_t std_storage_select( uint8_t *name );

	// returns storage id in use
	uint64_t std_storage_id( void );

	// return content of directory (in VFS structure)
	uintptr_t std_dir( uint8_t *path, uint64_t limit );

	// debug purposes only
	void std_log( uint8_t *string, uint64_t length );

	#ifdef	SOFTWARE
		struct	STD_STRUCTURE_ENTRY {
			uint64_t	length;
			uint8_t		*string;
		} __attribute__( (packed) );

		// function definitions

		// requests syscall and returns nothing
		void std_syscall_empty( void );

		// initial function of every process
		extern uint64_t _main( uint64_t argc, uint8_t *argv[] );

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

	// copy source content inside target
	void memcpy( uint8_t *target, uint8_t *source, uint64_t length ) { for( uint64_t i = 0; i < length; i++) target[ i ] = source[ i ]; }

	// fill cache with definied value
	void memset( uint8_t *cache, uint8_t value, uint64_t length ) { for( uint64_t i = 0; i < length; i++ ) cache[ i ] = value; }

	//------------------------------------------------------------------------------
	// substitute of libc
	//------------------------------------------------------------------------------

	typedef struct STD_STRUCTURE_FILE FILE;

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
	void print( const char *string );
	void printf( const char *string, ... );
	void sprintf( const char *string, ... );
	uint64_t pow( uint64_t base, uint64_t exponent );
	uint16_t getkey( void );
	void exit( void );
	FILE *fopen( uint8_t *path, uint8_t mode );
	void fclose( FILE *file );
	void fread( FILE *file, uint8_t *cache, uint64_t byte );
	void fwrite( FILE *file, uint8_t *cache, uint64_t byte );
	FILE *touch( uint8_t *path, uint8_t type );
	void sleep( uint64_t ms );
	void log( const char *string, ... );
#endif
