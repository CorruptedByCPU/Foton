/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	TIWYN_OBJECT_LIMIT			64
#define	TIWYN_LIST_LIMIT			(TIWYN_OBJECT_LIMIT + TRUE)
#define	TIWYN_ZONE_LIMIT			(TIWYN_OBJECT_LIMIT << STD_SHIFT_4)	// 4 edges per window

#define	TIWYN_PANEL_HEIGHT_pixel		22

struct	TIWYN_STRUCTURE_OBJECT {
	int16_t		x;
	int16_t		y;
	int16_t		width;
	int16_t		height;
	uint64_t	limit;
	uint64_t	pid;
	struct LIB_WINDOW_DESCRIPTOR	*descriptor;
};

struct	TIWYN_STRUCTURE_ZONE {
	int16_t		x;
	int16_t		y;
	int16_t		width;
	int16_t		height;
	struct TIWYN_STRUCTURE_OBJECT	*object;
};
