/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void module_usb_xhci_init( uint8_t c ) {
	// properties of controller registers
	volatile struct MODULE_USB_STRUCTURE_XHCI *xhci = (struct MODULE_USB_STRUCTURE_XHCI *) module_usb_controller[ c ].base_address;

	kernel -> log( (uint8_t *) "[USB].%u CAPLENGTH:  %2u\n", c, xhci -> length );
	kernel -> log( (uint8_t *) "[USB].%u HCIVERSION: 0x%4X\n", c, xhci -> version );
	kernel -> log( (uint8_t *) "[USB].%u HCSPARAMS1: %32bb\n", c, xhci -> structural_parameters_0 );
	kernel -> log( (uint8_t *) "[USB].%u HCSPARAMS2: %32bb\n", c, xhci -> structural_parameters_1 );
	kernel -> log( (uint8_t *) "[USB].%u HCSPARAMS3: %32bb\n", c, xhci -> structural_parameters_2 );
	kernel -> log( (uint8_t *) "[USB].%u HCCPARAMS1: %32bb (xECP => 0x%8X)\n", c, xhci -> capability_parameters_0, module_usb_controller[ c ].base_address + ((xhci -> capability_parameters_0 >> STD_SHIFT_65536) << STD_SHIFT_4) );
	kernel -> log( (uint8_t *) "[USB].%u DBOFF:      0x%8X\n", c, xhci -> doorbell_offset & ~0b00000011 );
	kernel -> log( (uint8_t *) "[USB].%u RTSOFF:     0x%8X\n", c, xhci -> runtime_offset & ~0b00001111 );

}