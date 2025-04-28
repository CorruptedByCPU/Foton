/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	WM_OBJECT_LIMIT			64
#define	WM_LIST_LIMIT			(WM_OBJECT_LIMIT + TRUE)
#define	WM_ZONE_LIMIT			(WM_OBJECT_LIMIT << STD_SHIFT_4)	// 4 edges per window

struct	WM_STRUCTURE_OBJECT {
	int16_t		x;
	int16_t		y;
	int16_t		width;
	int16_t		height;
	uint64_t	limit;
	uint64_t	pid;
	struct LIB_WINDOW_DESCRIPTOR	*descriptor;
};

struct	WM_STRUCTURE_ZONE {
	int16_t		x;
	int16_t		y;
	int16_t		width;
	int16_t		height;
	struct WM_STRUCTURE_OBJECT *object;
};
