/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	MODULE_USB_XHCI
	#define	MODULE_USB_XHCI

	struct	MODULE_USB_STRUCTURE_XHCI {
		uint8_t		length;
		uint8_t		reserved0;
		uint16_t	version;
		uint32_t	structural_parameters_0;
		uint32_t	structural_parameters_1;
		uint32_t	structural_parameters_2;
		uint32_t	capability_parameters_0;
		uint32_t	doorbell_offset;
		uint32_t	runtime_offset;
	} __attribute__( (packed) );

	void module_usb_xhci_init( uint8_t c );
#endif