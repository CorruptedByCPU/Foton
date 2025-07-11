/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	LIB_INTERFACE
	#define	LIB_INTERFACE

	#ifndef	LIB_ELF
		#include	"./elf.h"
	#endif
	#ifndef	LIB_FLOAT
		#include	"./float.h"
	#endif
	#ifndef	LIB_FONT
		#include	"./font.h"
	#endif
	#ifndef	LIB_IMAGE
		#include	"./image.h"
	#endif
	#ifndef	LIB_INPUT
		#include	"./input.h"
	#endif
	#ifndef	LIB_INTEGER
		#include	"./integer.h"
	#endif
	#ifndef	LIB_JSON
		#include	"./json.h"
	#endif
	#ifndef	LIB_STRING
		#include	"./string.h"
	#endif
	#ifndef	LIB_TYPE
		#include	"./type.h"
	#endif
	#ifndef	LIB_VFS
		#include	"./vfs.h"
	#endif
	#ifndef	LIB_WINDOW
		#include	"./window.h"
	#endif

	#define	LIB_INTERFACE_GLOBAL_NAME_limit				64

	#define	LIB_INTERFACE_LATENCY_microtime				256

	#define	LIB_INTERFACE_HEADER_HEIGHT_pixel			(LIB_FONT_HEIGHT_pixel + 8)	// 4 pixels from above and under

	#define	LIB_INTERFACE_ELEMENT_MENU_HEIGHT_pixel			LIB_INTERFACE_HEADER_HEIGHT_pixel

	#define	LIB_INTERFACE_ELEMENT_TYPE_null				0x00
	#define	LIB_INTERFACE_ELEMENT_TYPE_label			0x01
	#define	LIB_INTERFACE_ELEMENT_TYPE_button			0x02
	#define	LIB_INTERFACE_ELEMENT_TYPE_control_close		0x03
	#define	LIB_INTERFACE_ELEMENT_TYPE_control_maximize		0x04
	#define	LIB_INTERFACE_ELEMENT_TYPE_control_minimize		0x05
	#define	LIB_INTERFACE_ELEMENT_TYPE_menu				0x06
	#define	LIB_INTERFACE_ELEMENT_TYPE_input			0x07
	#define	LIB_INTERFACE_ELEMENT_TYPE_checkbox			0x08
	#define	LIB_INTERFACE_ELEMENT_TYPE_radio			0x09
	#define	LIB_INTERFACE_ELEMENT_TYPE_scroll			0x0A
	#define	LIB_INTERFACE_ELEMENT_TYPE_list				0x0C

	#define	LIB_INTERFACE_ELEMENT_FLAG_hover			0x01
	#define	LIB_INTERFACE_ELEMENT_FLAG_active			0x02
	#define	LIB_INTERFACE_ELEMENT_FLAG_flush			0x04

	#define	LIB_INTERFACE_ELEMENT_LIST_FLAG_persistent		0x01	// disard deselection of entry
	#define	LIB_INTERFACE_ELEMENT_LIST_FLAG_individual		0x02	// only 1 can be selected at a time
	#define	LIB_INTERFACE_ELEMENT_LIST_FLAG_single_click		0x04

	#define	LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_hover		0x01
	#define	LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_active		0x02
	#define	LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_run		0x04
	#define	LIB_INTERFACE_ELEMENT_LIST_ENTRY_FLAG_select		0x08

	#define	LIB_INTERFACE_BORDER_pixel				1
	#define	LIB_INTERFACE_BORDER_COLOR_default			0xFF282828
	#define	LIB_INTERFACE_BORDER_COLOR_default_shadow		0xFF202020
	#define	LIB_INTERFACE_BORDER_COLOR_inactive			LIB_INTERFACE_BORDER_COLOR_default_shadow
	#define	LIB_INTERFACE_BORDER_COLOR_inactive_shadow		0xFF181818
	#define	LIB_INTERFACE_BORDER_COLOR_internal			0xFF2B2B2B

	#define	LIB_INTERFACE_COLOR_background				0xFF181818
	#define	LIB_INTERFACE_COLOR_background_lighter			0xFF303030
	#define	LIB_INTERFACE_COLOR_background_button_default		0xFF303030
	#define	LIB_INTERFACE_COLOR_background_menu_default		0xFF1F1F1F
	#define	LIB_INTERFACE_COLOR_background_menu_selected		LIB_INTERFACE_COLOR_background_menu_default + 0x00101010;
	#define	LIB_INTERFACE_COLOR_background_input_default		0xFF242424;
	#define	LIB_INTERFACE_COLOR_background_input_selected		LIB_INTERFACE_COLOR_background_input_default + 0x00101010;
	#define	LIB_INTERFACE_COLOR_background_radio_default		LIB_INTERFACE_COLOR_background + 0x00101010;
	#define	LIB_INTERFACE_COLOR_background_radio_selected		0xFF202080
	#define	LIB_INTERFACE_COLOR_background_checkbox_default		LIB_INTERFACE_COLOR_background + 0x00101010;
	#define	LIB_INTERFACE_COLOR_background_checkbox_selected	0xFF208020
	#define	LIB_INTERFACE_COLOR_background_control_close_hover	0xFF00CC00
	#define	LIB_INTERFACE_COLOR_background_file_default		0xFF1F1F1F
	#define	LIB_INTERFACE_COLOR_background_file_odd			0xFF161616
	#define	LIB_INTERFACE_COLOR_background_file_selected		0xFF208020
	#define	LIB_INTERFACE_COLOR_foreground				0xFFF0F0F0

	#define	LIB_INTERFACE_NAME_limit				LIB_INTERFACE_GLOBAL_NAME_limit

	struct LIB_INTERFACE_STRUCTURE {
		struct LIB_WINDOW_STRUCTURE	*descriptor;
		uint8_t		*properties;
		//--------------------------------------------------
		int16_t		x;
		int16_t		y;
		uint16_t	width;
		uint16_t	height;
		//--------------------------------------------------
		int16_t		previous_x;
		int16_t		previous_y;
		uint16_t	previous_width;
		uint16_t	previous_height;
		//--------------------------------------------------
		uint16_t	min_width;
		uint16_t	min_height;
		//--------------------------------------------------
		uint8_t		controls;
		uint8_t		active_semaphore;
		struct LIB_INTERFACE_STRUCTURE_ELEMENT	*element_select;
		uint32_t	background_color;	// if set (alpha channel set), choose as background color
		//--------------------------------------------------
		uint8_t		key_alt_semaphore;
		uint8_t		key_ctrl_semaphore;
		uint8_t		key_shift_semaphore;
		//--------------------------------------------------
		uint8_t		name_length;
		uint8_t		name[ LIB_INTERFACE_NAME_limit ];
	};

	struct	LIB_INTERFACE_STRUCTURE_ELEMENT {
		uint8_t		type;
		uint8_t		flags;
		uint16_t	size_byte;	// element size (depends of type)
		int16_t		x;
		int16_t		y;
		uint16_t	width;
		uint16_t	height;
		uint16_t	id;
		uint8_t		active;
		uint8_t		selected;
		uint8_t		group;
		void		(*function)( void );
	};

	struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL {
		struct LIB_INTERFACE_STRUCTURE_ELEMENT	control;
		void		(*event)( void );
	};

	struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON {
		struct LIB_INTERFACE_STRUCTURE_ELEMENT	label_or_button;
		uint16_t	name_length;
		uint8_t		*name;
		void		(*event)( void );
	};

	struct LIB_INTERFACE_STRUCTURE_ELEMENT_CHECKBOX {
		struct LIB_INTERFACE_STRUCTURE_ELEMENT	checkbox;
		uint16_t	name_length;
		uint8_t		*name;
	};

	struct LIB_INTERFACE_STRUCTURE_ELEMENT_RADIO {
		struct LIB_INTERFACE_STRUCTURE_ELEMENT	radio;
		uint16_t	name_length;
		uint8_t		*name;
	};

	struct LIB_INTERFACE_STRUCTURE_ELEMENT_INPUT {
		struct LIB_INTERFACE_STRUCTURE_ELEMENT	input;
		uint16_t	name_length;
		uint8_t		*name;
		uint64_t	offset;
		uint64_t	index;
	};

	struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU {
		struct LIB_INTERFACE_STRUCTURE_ELEMENT	menu;
		uint16_t	name_length;
		uint8_t		*name;
		void		(*event)( struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU *menu );
		uint8_t		*command;
		uint32_t	*icon;
	};

	struct	LIB_INTERFACE_STRUCTURE_ELEMENT_FILE {
		struct	LIB_INTERFACE_STRUCTURE_ELEMENT			file;
		struct	LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY	*entry;
		uint8_t		flags;
		uint8_t		view;
		uint32_t	*pixel;
		uint64_t	limit;
		uint64_t	offset;
		uint64_t	microtime;
		uint32_t	color_default;
		uint32_t	color_odd;
		uint32_t	color_selected;
	};

	struct	LIB_INTERFACE_STRUCTURE_ELEMENT_FILE_ENTRY {
		uint64_t	id;
		uint8_t		flags;
		uint8_t		type;
		uint8_t		mimetype;
		uint64_t	byte;
		uint8_t		name_length;
		uint8_t		*name;
		uint32_t	*icon;
	};

	struct LIB_INTERFACE_STRUCTURE_SELECT {
		struct LIB_INTERFACE_STRUCTURE_ELEMENT	*previous;
		struct LIB_INTERFACE_STRUCTURE_ELEMENT	*next;
	};

	// properties of Interface assigned to Window
	uint8_t lib_interface( struct LIB_INTERFACE_STRUCTURE *interface );

	// fill interface background with default colors
	void lib_interface_clear( struct LIB_INTERFACE_STRUCTURE *interface );

	// changes JSON format to internal Interface format
	void lib_interface_convert( struct LIB_INTERFACE_STRUCTURE *interface );

	// parse all elements of interface and show them
	void lib_interface_draw( struct LIB_INTERFACE_STRUCTURE *interface );

	// find element properties by its ID
	uintptr_t lib_interface_element_by_id( struct LIB_INTERFACE_STRUCTURE *interface, uint64_t id );

	// show control element of definied properties
	void lib_interface_element_control( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_CONTROL *control );

	// show label element of definied properties
	void lib_interface_element_label( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *element );

	void lib_interface_element_button( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_LABEL_OR_BUTTON *element );

	void lib_interface_element_input( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_INPUT *element );

	void lib_interface_element_checkbox( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_CHECKBOX *element );

	// show menu element of definied properties
	void lib_interface_element_menu( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_MENU *element );

	void lib_interface_element_radio( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_RADIO *element );

	// check incomming events
	struct LIB_INTERFACE_STRUCTURE *lib_interface_event( struct LIB_INTERFACE_STRUCTURE *interface );
	// support function of lib_interface_event, performed exclusively by WM
	void lib_interface_event_handler_release( struct LIB_INTERFACE_STRUCTURE *interface );
	uint16_t lib_interface_event_keyboard( struct LIB_INTERFACE_STRUCTURE *interface );

	void lib_interface_event_handler_press( struct LIB_INTERFACE_STRUCTURE *interface );

	// change status of elements of interface
	void lib_interface_active_or_hover( struct LIB_INTERFACE_STRUCTURE *interface, int16_t scroll );

	void lib_interface_element_file( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT_FILE *element );

	// rename window header
	void lib_interface_name( struct LIB_INTERFACE_STRUCTURE *interface );

	// update name on window interface
	void lib_interface_name_rewrite( struct LIB_INTERFACE_STRUCTURE *interface );

	void lib_interface_draw_select( struct LIB_INTERFACE_STRUCTURE *interface, struct LIB_INTERFACE_STRUCTURE_ELEMENT *element );

	uint64_t lib_interface_string( uint8_t font_family, uint8_t *string, uint64_t limit, uint64_t pixel );

	uint32_t *lib_interface_icon( uint8_t *path );

	struct LIB_INTERFACE_STRUCTURE_SELECT lib_interface_select( struct LIB_INTERFACE_STRUCTURE *interface );
#endif
