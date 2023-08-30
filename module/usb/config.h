/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	DRIVER_USB
	#define	DRIVER_USB

	#define	DRIVER_USB_BASE_ADDRESS_type	0b0001

	#define	DRIVER_USB_CONTROLLER_limit	1

	struct DRUVER_USB_CONTROLLER_STRUCTURE {
		uint8_t		type;
		uintptr_t	base_address;
		
	};
#endif