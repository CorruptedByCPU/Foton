/*=============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
=============================================================================*/

#ifndef	STD
	#define	STD

	// definitions, that are always nice to have
	#include	"stdint.h"
	#include	"stddef.h"
	#include	"stdarg.h"
	#include	"macro.h"

	#define	EMPTY						0

	#define	TRUE						1
	#define	FALSE						0

	#define	LOCK						TRUE
	#define	UNLOCK						FALSE

	#define	STD_VIDEO_DEPTH_shift				2
	#define	STD_VIDEO_DEPTH_byte				4
	#define	STD_VIDEO_DEPTH_bit				32

	#define	STD_COLOR_mask					0xFF000000
	#define	STD_COLOR_WHITE					0xFFFFFFFF
	#define	STD_COLOR_BLACK_light				0xFF101010
	#define	STD_COLOR_GREEN_light				0xFF00FF00
#endif
