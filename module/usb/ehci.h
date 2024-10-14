/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	MODULE_USB_EHCI
	#define	MODULE_USB_EHCI

	struct	MODULE_USB_STRUCTURE_EHCI_REGISTER {
		uint8_t		length;
		uint8_t		reserved;
		uint16_t	version;
		uint32_t	structure;
		uint32_t	capability;
		uint64_t	port_route;
	} __attribute__( (packed) );

	struct MODULE_USB_STRUCTURE_EHCI_CAPABILITY {
		uint8_t		long_mode		: 1;
		uint8_t		programmable_frame_list	: 1;
		uint8_t		asynchronous		: 1;
		uint8_t		reserved		: 1;
		uint8_t		isochronous		: 4;
		uint8_t		extended_capabilities;
	} __attribute__( (packed) );

	struct MODULE_USB_STRUCTURE_EHCI_OPERATIONAL_REGISTER {
		uint32_t	command;
		uint32_t	status;
		uint32_t	interrupt_enable;
		uint32_t	frame_index;
		uint32_t	segment_selector;
		uint32_t	frame_list_base_address;
		uint32_t	asynchronous_list_address;
		uint32_t	reserved[ 9 ];
		uint32_t	config;
		uint32_t	port[ TRUE ];	// by default 2, but correct amount is determined by MODULE_USB_STRUCTURE_EHCI_REGISTER.structure (HCPARAMS)
	} __attribute__( (packed) );

	void module_usb_ehci_init( uint8_t c );
#endif