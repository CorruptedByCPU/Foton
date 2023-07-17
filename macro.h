/*=============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
=============================================================================*/

#ifndef	MACRO
	#define MACRO

	#define	MACRO_DEBUF() { __asm__ volatile( "xchg %bx, %bx" ); }
#endif
