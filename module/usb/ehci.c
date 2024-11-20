/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void module_usb_ehci_init( uint8_t c ) {
	// properties of controller registers
	volatile struct MODULE_USB_STRUCTURE_EHCI_REGISTER *ehci = (struct MODULE_USB_STRUCTURE_EHCI_REGISTER *) module_usb_controller[ c ].base_address;

	// disable BIOS legacy support
	volatile struct MODULE_USB_STRUCTURE_EHCI_CAPABILITY *eecp = (struct MODULE_USB_STRUCTURE_EHCI_CAPABILITY *) &ehci -> capability;
	if( eecp -> extended_capabilities >= 0x40 && (driver_pci_read( module_usb_controller[ c ].pci, eecp -> extended_capabilities ) & 0xFF) == 0x01 ) {
		// i'm the captain now
		driver_pci_write( module_usb_controller[ c ].pci, eecp -> extended_capabilities, 1 << 24 );

		// debug
		kernel -> log( (uint8_t *) "[USB].%u PCI %2X:%2X.%u - BIOS Legacy Support, disabled.\n", c, module_usb_controller[ c ].pci.bus, module_usb_controller[ c ].pci.device, module_usb_controller[ c ].pci.function );
	}

	// properties of EHCI Operational Registers
	volatile struct MODULE_USB_STRUCTURE_EHCI_OPERATIONAL_REGISTER *op = (struct MODULE_USB_STRUCTURE_EHCI_OPERATIONAL_REGISTER *) (module_usb_controller[ c ].base_address + ehci -> length);

	// hard reset of controller
	op -> command = 1 << 1;

	// wait for command finish
	kernel -> time_sleep( 8 );

	// nothing more, leave ports to companion controllers if they exist
	// op -> config = EMPTY;
}