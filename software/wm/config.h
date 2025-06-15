/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	WM_OBJECT_LIMIT				64
#define	WM_LIST_LIMIT				(WM_OBJECT_LIMIT + TRUE)
#define	WM_ZONE_LIMIT				(WM_OBJECT_LIMIT << STD_SHIFT_32)

#define	WM_OBJECT_OVERSHADE_COLOR		0x20008000
#define	WM_OBJECT_OVERSHADE_COLOR_BORDER	WM_OBJECT_OVERSHADE_COLOR + 0x80707070

#define	WM_PANEL_COLOR_default			0xFF202020
#define	WM_PANEL_COLOR_visible			0xFF0C0C0C
#define	WM_PANEL_COLOR_active			0xFF00CC00
#define	WM_PANEL_CLOCK_WIDTH_pixel		50
#define	WM_PANEL_HEIGHT_pixel			22
#define	WM_PANEL_ENTRY_WIDTH_limit		256

struct	WM_STRUCTURE_OBJECT {
	int16_t		x;
	int16_t		y;
	int16_t		z;
	int16_t		width;
	int16_t		height;
	uint64_t	limit;
	uint64_t	pid;
	struct LIB_WINDOW_STRUCTURE	*descriptor;
};

struct	WM_STRUCTURE_ZONE {
	int16_t		x;
	int16_t		y;
	int16_t		z;
	int16_t		width;
	int16_t		height;
	struct WM_STRUCTURE_OBJECT	*object;
};

struct	WM_STRUCTURE {
	// by default FALSE, until Desktop Environment - end of initialization
	uint8_t						enable;

	// process ID of Window Manager
	uint64_t					pid;

	struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER	framebuffer;

	// array off all "windows"
	struct WM_STRUCTURE_OBJECT			*object;
	// list off all objects in order of creation time or Z axis
	struct WM_STRUCTURE_OBJECT			**list;
	struct WM_STRUCTURE_OBJECT			**list_panel;
	// list of fragments of object to parse
	struct WM_STRUCTURE_ZONE			*zone;

	uint64_t					list_start;

	// lists length
	uint64_t					list_limit;
	uint64_t					list_limit_panel;
	volatile uint64_t				zone_limit;

	// do not allow modification of object array/list
	uint8_t						object_lock;
	uint8_t						list_lock;

	// draw on canvas, before sync with Graphics Card
	struct WM_STRUCTURE_OBJECT			canvas;

	// currently active object, which will receive keyboard/mouse input
	struct WM_STRUCTURE_OBJECT			*active;
	struct WM_STRUCTURE_OBJECT			*selected;
	struct WM_STRUCTURE_OBJECT			*resized;
	struct WM_STRUCTURE_OBJECT			*shade;

	uint64_t					panel_entry_width;

	// default objects
	struct WM_STRUCTURE_OBJECT			*workbench;
	struct WM_STRUCTURE_OBJECT			*panel;
	struct WM_STRUCTURE_OBJECT			*menu;
	struct WM_STRUCTURE_OBJECT			*cursor;

	struct WM_STRUCTURE_ZONE			direction;

	uint64_t					panel_clock_state;

	// semaphores
	uint8_t						panel_semaphore;
	uint8_t						drag_allow;
	uint8_t 					key_menu;
	uint8_t						mouse_button_left;
	uint8_t						mouse_button_right;
	uint8_t						shade_initialized;
};
