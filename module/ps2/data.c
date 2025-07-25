/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct KERNEL *kernel = EMPTY;

uint8_t module_ps2_mouse_semaphore		= FALSE;
uint8_t module_ps2_mouse_type			= EMPTY;
uint8_t module_ps2_mouse_package_id		= EMPTY;

uint8_t module_ps2_keyboard_matrix = FALSE;

volatile uint16_t module_ps2_scancode = EMPTY;

uint16_t	module_ps2_keyboard_matrix_low[] = {
	EMPTY,
	STD_KEY_ESC,	// Escape
	0x0031,	// 1
	0x0032,	// 2
	0x0033,	// 3
	0x0034,	// 4
	0x0035,	// 5
	0x0036,	// 6
	0x0037,	// 7
	0x0038,	// 8
	0x0039,	// 9
	0x0030,	// 0
	0x002D,	// -
	0x003D,	// =
	STD_KEY_BACKSPACE,
	STD_KEY_TAB,
	0x0071,	// q
	0x0077,	// w
	0x0065,	// e
	0x0072,	// r
	0x0074,	// t
	0x0079,	// y
	0x0075,	// u
	0x0069,	// i
	0x006F,	// o
	0x0070,	// p
	0x005B,	// [
	0x005D,	// ]
	STD_KEY_ENTER,
	STD_KEY_CTRL_LEFT,
	0x0061,	// a
	0x0073,	// s
	0x0064,	// d
	0x0066,	// f
	0x0067,	// g
	0x0068,	// h
	0x006A,	// j
	0x006B,	// k
	0x006C,	// l
	0x003B,	// ;
	0x0027,	// '
	0x0060,	// `
	STD_KEY_SHIFT_LEFT,
	0x005C,	// BACKSLASH
	0x007A,	// z
	0x0078,	// x
	0x0063,	// c
	0x0076,	// v
	0x0062,	// b
	0x006E,	// n
	0x006D,	// m
	0x002C,	// ,
	0x002E,	// .
	0x002F,	// /
	STD_KEY_SHIFT_RIGHT,
	STD_KEY_NUMLOCK_MULTIPLY,
	STD_KEY_ALT_LEFT,
	STD_KEY_SPACE,
	STD_KEY_CAPSLOCK,
	STD_KEY_F1,
	STD_KEY_F2,
	STD_KEY_F3,
	STD_KEY_F4,
	STD_KEY_F5,
	STD_KEY_F6,
	STD_KEY_F7,
	STD_KEY_F8,
	STD_KEY_F9,
	STD_KEY_F10,
	STD_KEY_NUMLOCK,
	STD_KEY_SCROLL_LOCK,
	EMPTY,
	EMPTY,
	EMPTY,
	STD_KEY_NUMLOCK_7,
	STD_KEY_NUMLOCK_8,
	STD_KEY_NUMLOCK_9,
	STD_KEY_NUMLOCK_MINUS,
	STD_KEY_NUMLOCK_4,
	STD_KEY_NUMLOCK_5,
	STD_KEY_NUMLOCK_6,
	STD_KEY_NUMLOCK_PLUS,
	STD_KEY_NUMLOCK_1,
	STD_KEY_NUMLOCK_2,
	STD_KEY_NUMLOCK_3,
	STD_KEY_NUMLOCK_0,
	STD_KEY_NUMLOCK_DOT,
	EMPTY,
	EMPTY,
	EMPTY,
	EMPTY,
	STD_KEY_F11,
	STD_KEY_F12
};

uint16_t module_ps2_keyboard_matrix_high[] = {
	EMPTY,
	STD_KEY_ESC,
	0x0021,	// !
	0x0040,	// @
	0x0023,	// #
	0x0024,	// $
	0x0025,	// %
	0x005E,	// ^
	0x0026,	// &
	0x002A,	// *
	0x0028,	// ()
	0x0029,	// )
	0x005F,	// _
	0x002B,	// +
	STD_KEY_BACKSPACE,
	STD_KEY_TAB,
	0x0051,	// Q
	0x0057,	// W
	0x0045,	// E
	0x0052,	// R
	0x0054,	// T
	0x0059,	// Y
	0x0055,	// U
	0x0049,	// I
	0x004F,	// O
	0x0050,	// P
	0x007B,	// }
	0x007D,	// {
	STD_KEY_ENTER,
	STD_KEY_CTRL_LEFT,
	0x0041,	// A
	0x0053,	// S
	0x0044,	// D
	0x0046,	// F
	0x0047,	// G
	0x0048,	// H
	0x004A,	// J
	0x004B,	// K
	0x004C,	// L
	0x003A,	// :
	0x0022,	// "
	0x007E,	// ~
	STD_KEY_SHIFT_LEFT,
	0x007C,	// |
	0x005A,	// Z
	0x0058,	// X
	0x0043,	// C
	0x0056,	// V
	0x0042,	// B
	0x004E,	// N
	0x004D,	// M
	0x003C,	// <
	0x003E,	// >
	0x003F,	// ?
	STD_KEY_SHIFT_RIGHT,
	STD_KEY_NUMLOCK_MULTIPLY,
	STD_KEY_ALT_LEFT,
	STD_KEY_SPACE,
	STD_KEY_CAPSLOCK,
	STD_KEY_F1,
	STD_KEY_F2,
	STD_KEY_F3,
	STD_KEY_F4,
	STD_KEY_F5,
	STD_KEY_F6,
	STD_KEY_F7,
	STD_KEY_F8,
	STD_KEY_F9,
	STD_KEY_F10,
	STD_KEY_NUMLOCK,
	STD_KEY_SCROLL_LOCK,
	EMPTY,
	EMPTY,
	EMPTY,
	STD_KEY_NUMLOCK_7,
	STD_KEY_NUMLOCK_8,
	STD_KEY_NUMLOCK_9,
	STD_KEY_NUMLOCK_MINUS,
	STD_KEY_NUMLOCK_4,
	STD_KEY_NUMLOCK_5,
	STD_KEY_NUMLOCK_6,
	STD_KEY_NUMLOCK_PLUS,
	STD_KEY_NUMLOCK_1,
	STD_KEY_NUMLOCK_2,
	STD_KEY_NUMLOCK_3,
	STD_KEY_NUMLOCK_0,
	STD_KEY_NUMLOCK_DOT,
	EMPTY,
	EMPTY,
	EMPTY,
	EMPTY,
	STD_KEY_F11,
	STD_KEY_F12
};
