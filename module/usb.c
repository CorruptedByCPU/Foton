/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions of kernel
	//----------------------------------------------------------------------
	#include	"../kernel/config.h"
	//----------------------------------------------------------------------
	// drivers
	//----------------------------------------------------------------------
	#include	"../kernel/driver/port.h"
	#include	"../kernel/driver/port.c"
	#include	"../kernel/driver/pci.h"
	#include	"../kernel/driver/pci.c"
	//----------------------------------------------------------------------
	// variables, structures, definitions of module
	//----------------------------------------------------------------------
	#include	"./usb/config.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./usb/data.c"

void _entry( struct KERNEL *kernel ) {
	// locate Universal Serial Bus controller
	struct DRIVER_PCI_STRUCTURE pci = driver_pci_find_class_and_subclass( DRIVER_PCI_CLASS_SUBCLASS_usb );

	// USB controller found?
	if( ! pci.result ) return;	// no

	// show information about controller
	kernel -> log( (uint8_t *) "[usb.ko] PCI %2X:%2X.%u - Universal Serial Bus controller found.\n", pci.bus, pci.device, pci.function );

	// type different than UHCI?
	uint64_t base_address_space = driver_pci_read( pci, DRIVER_PCI_REGISTER_bar4 );
	if( ! base_address_space ) return;	// yep, no support

	// check type of base address space
	uint8_t *base_address_type = (uint8_t *) &driver_usb_string_memory;
	if( base_address_space & DRIVER_USB_BASE_ADDRESS_type ) base_address_type = (uint8_t *) &driver_usb_string_port;

	// get size of base address space
	driver_pci_write( pci, DRIVER_PCI_REGISTER_bar4, 0xFFFFFFFF );
	uint32_t base_address_size = ~(driver_pci_read( pci, DRIVER_PCI_REGISTER_bar4 ) & ~1 ) + 1;
	// restore original value
	driver_pci_write( pci, DRIVER_PCI_REGISTER_bar4, base_address_space );

	// retrieve base address space configuration and truncate
	uint8_t base_address_config = base_address_space & 0x0F; base_address_space &= ~0x0F;

	// base address space should be 64bit?
	if( base_address_config & 0b0100 ) base_address_space |= (uint64_t) driver_pci_read( pci, DRIVER_PCI_REGISTER_bar5 ) << 32;

	// show properties of device
	kernel -> log( (uint8_t *) "[usb.ko]  Type: UHCI (Universal Host Controller Interface)\n[usb.ko]  I/O %s at 0x%X [0x%X Bytes], I/O APIC line %u.\n", base_address_type, base_address_space, base_address_size, driver_pci_read( pci, DRIVER_PCI_REGISTER_irq ) & 0x0F );

	// hold the door
	while( TRUE );
}