/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

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
	#define	STD_COLOR_WHITE					0xFFF0F0F0
	#define	STD_COLOR_BLACK					0xFF101010
	#define	STD_COLOR_BLACK_light				0xFF101010
	#define	STD_COLOR_GREEN_light				0xFF10FF10
	#define	STD_COLOR_RED_light				0xFFFF1010

	#define	STD_SHIFT_4					2
	#define	STD_SHIFT_8					3
	#define	STD_SHIFT_16					4
	#define	STD_SHIFT_32					5
	#define	STD_SHIFT_64					6
	#define	STD_SHIFT_PTR					STD_SHIFT_64
	#define	STD_SHIFT_512					9
	#define	STD_SHIFT_1024					10
	#define	STD_SHIFT_PAGE					12

	#define	STD_PAGE_byte					0x1000
	#define	STD_PAGE_mask					0xFFFFFFFFFFFFF000
#endif
