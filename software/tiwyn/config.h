/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	TIWYN_OBJECT_LIMIT		64
#define	TIWYN_LIST_LIMIT		(TIWYN_OBJECT_LIMIT + TRUE)
#define	TIWYN_ZONE_LIMIT		(TIWYN_OBJECT_LIMIT << STD_SHIFT_4)	// 4 edges per window

struct	TIWYN_STRUCTURE_OBJECT {
	int16_t		x;
	int16_t		y;
	int16_t		z;
	int16_t		width;
	int16_t		height;
	uint64_t	limit;
	uint64_t	pid;
	struct LIB_WINDOW_DESCRIPTOR	*descriptor;
};

struct	TIWYN_STRUCTURE_ZONE {
	int16_t		x;
	int16_t		y;
	int16_t		z;
	int16_t		width;
	int16_t		height;
	struct TIWYN_STRUCTURE_OBJECT	*object;
};

struct	TIWYN_STRUCTURE {
	// by default FALSE, until Desktop Environment - end of initialization
	uint8_t						enable;

	// process ID of Tiwyn
	uint64_t					pid;

	struct STD_STRUCTURE_SYSCALL_FRAMEBUFFER	framebuffer;

	// array off all "windows"
	struct TIWYN_STRUCTURE_OBJECT			*object;
	// list off all objects in order of creation time or Z axis
	struct TIWYN_STRUCTURE_OBJECT			**list;
	// list of fragments of object to parse
	struct TIWYN_STRUCTURE_ZONE			*zone;

	// lists length
	uint64_t					list_limit;
	uint64_t					zone_limit;

	// draw on canvas, before sync with Graphics Card
	struct TIWYN_STRUCTURE_OBJECT			canvas;

	// currently active object, which will receive keyboard/mouse input
	struct TIWYN_STRUCTURE_OBJECT			*active;
	struct TIWYN_STRUCTURE_OBJECT			*selected;

	// integral part of Tiwyn
	struct TIWYN_STRUCTURE_OBJECT			*cursor;

	// semaphores
	uint8_t						drag_allow;
	uint8_t 					key_ctrl_left;
	uint8_t						mouse_button_left;
	uint8_t						mouse_button_right;
};
