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

	void module_usb_ehci_init( uint8_t c );
#endif