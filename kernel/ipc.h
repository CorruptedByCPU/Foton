/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_IPC
	#define	KERNEL_IPC

	#define	KERNEL_IPC_limit	512
	#define	KERNEL_IPC_ttl		DRIVER_RTC_Hz >> STD_SHIFT_3	// about 1/8 of second
#endif