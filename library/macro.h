/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	MACRO
	#define MACRO

	#define	MACRO_DEBUF( void ) { __asm__ volatile( "xchg %bx, %bx" ); }

	#define	MACRO_PAGE_ALIGN_UP( value )(((value) + STD_PAGE_byte - 1) & ~(STD_PAGE_byte - 1))
	#define	MACRO_PAGE_ALIGN_DOWN( value )((value) & ~(STD_PAGE_byte - 1))
#endif
