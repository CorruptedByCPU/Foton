/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	DRIVER_PS2
	#define	DRIVER_PS2

	// external routines (assembly language)
	extern void driver_ps2_mouse_entry( void );
	extern void driver_ps2_keyboard_entry( void );

	__attribute__(( no_caller_saved_registers )) void driver_ps2_mouse( void );
	__attribute__(( no_caller_saved_registers )) void driver_ps2_keyboard( void );
#endif