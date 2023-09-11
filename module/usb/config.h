/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	DRIVER_USB
	#define	DRIVER_USB

	#define	DRIVER_USB_BASE_ADDRESS_type	0b0001

	#define	DRIVER_USB_CONTROLLER_limit	1
	#define	DRIVER_USB_PORT_limit		2

	#define	DRIVER_USB_PORT_current_connection_status	0
	#define	DRIVER_USB_PORT_connection_status_change	1
	#define	DRIVER_USB_PORT_port_enabled			2
	#define	DRIVER_USB_PORT_port_enable_change		3

	#define	DRIVER_USB_PORT_FLAG_reserved			0b00000001

	#define	DRIVER_USB_FRAME_FLAG_terminate			0b0001
	#define	DRIVER_USB_FRAME_FLAG_queue			0b0010

	struct DRIVER_USB_CONTROLLER_STRUCTURE {
		uint8_t		type;
		uintptr_t	base_address;
		uint16_t	size_byte;
		uint64_t	frame_base_address;
		uint8_t		irq_line;
	};

	struct DRIVER_USB_PORT_STRUCTURE {
		uint8_t		flags;
		uint8_t		controller_id;
		uint8_t		port_id;
	};

	struct DRIVER_USB_UHCI_STRUCTURE_REGISTER {
		uint16_t	command;
		uint16_t	status;
		uint16_t	interrupt;
		uint16_t	frame_number;
		uint32_t	frame_base_address;
		uint8_t		modify;
		uint8_t		reserved[ 3 ];
		uint16_t	port[ 2 ];	// by default 2, but correct amount is determined by DRIVER_USB_CONTROLLER_STRUCTURE.size_byte
	} __attribute__( (packed) );
#endif