/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	DRIVER_USB
	#define	DRIVER_USB

	#define	DRIVER_USB_BASE_ADDRESS_type	0b0001

	#define	DRIVER_USB_CONTROLLER_limit	1

	struct DRIVER_USB_CONTROLLER_STRUCTURE {
		uint8_t		type;
		uintptr_t	base_address;
		uint64_t	frame_base_address;
		uint8_t		irq_line;
	};

	struct DRIVER_USB_STRUCTURE_REGISTER {
		uint16_t	command;
		uint16_t	status;
		uint16_t	interrupt;
		uint16_t	frame_number;
		uint32_t	frame_base_address;
		uint8_t		modify;
		uint8_t		reserved[ 3 ];
		uint16_t	port0;
		uint16_t	port1;
	} __attribute__( (packed) );
#endif