/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	DRIVER_USB_BASE_ADDRESS_type	0b0001

uint8_t driver_usb_string_port[] = "port";
uint8_t driver_usb_string_memory[] = "memory";

void _entry( void ) {
	// // locate Universal Serial Bus controller
	// struct DRIVER_PCI_STRUCTURE pci = driver_pci_find_class_and_subclass( DRIVER_PCI_CLASS_SUBCLASS_usb );

	// // USB controller found?
	// if( ! pci.result ) return;	// no

	// // show information about controller
	// lib_terminal_printf( &kernel_terminal, (uint8_t *) "PCI %2X:%2X.%u - Universal Serial Bus controller.\n", pci.bus, pci.device, pci.function );

	// // type different than UHCI?
	// uint64_t base_address_space = driver_pci_read( pci, DRIVER_PCI_REGISTER_bar4 );
	// if( ! base_address_space ) return;	// yep, no support

	// // check type of base address space
	// uint8_t *base_address_type = (uint8_t *) &driver_usb_string_memory;
	// if( base_address_space & DRIVER_USB_BASE_ADDRESS_type ) base_address_type = (uint8_t *) &driver_usb_string_port;

	// // get size of base address space
	// driver_pci_write( pci, DRIVER_PCI_REGISTER_bar4, 0xFFFFFFFF );
	// uint32_t base_address_size = ~(driver_pci_read( pci, DRIVER_PCI_REGISTER_bar4 ) & ~1 ) + 1;
	// // restore original value
	// driver_pci_write( pci, DRIVER_PCI_REGISTER_bar4, base_address_space );

	// // retrieve base address space configuration and truncate
	// uint8_t base_address_config = base_address_space & 0x0F; base_address_space &= ~0x0F;

	// // base address space should be 64bit?
	// if( base_address_config & 0b0100 ) base_address_space |= (uint64_t) driver_pci_read( pci, DRIVER_PCI_REGISTER_bar5 ) << 32;

	// // show properties of device
	// lib_terminal_printf( &kernel_terminal, (uint8_t *) " Type: UHCI (Universal Host Controller Interface)\n I/O %s at 0x%X [0x%X Bytes], I/O APIC line %u.\n", base_address_type, base_address_space, base_address_size, driver_pci_read( pci, DRIVER_PCI_REGISTER_irq ) & 0x0F );
}