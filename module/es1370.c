/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions of standard library
	//----------------------------------------------------------------------
	#include	"../library/std.h"
	//----------------------------------------------------------------------
	// variables, structures, definitions of kernel
	//----------------------------------------------------------------------
	#include	"../kernel/config.h"
	#include	"../kernel/idt.h"
	#include	"../kernel/io_apic.h"
	#include	"../kernel/lapic.h"
	#include	"../kernel/page.h"
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
	#include	"./es1370/config.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./es1370/data.c"

void _entry( uintptr_t kernel_ptr ) {
	// preserve kernel structure pointer
	kernel = (struct KERNEL *) kernel_ptr;

	// find device of type "multimedia audio controller"
	struct DRIVER_PCI_STRUCTURE pci = driver_pci_find_class_and_subclass( DRIVER_PCI_CLASS_SUBCLASS_audio );

	// Ensoniq AudioPCI found?
	if( driver_pci_read( pci, DRIVER_PCI_REGISTER_vendor_and_device ) != DRIVER_ES1370_VENDOR_AND_DEVICE ) return;	// no

	// debug
	// kernel -> log( (uint8_t *) "[ES1370] PCI %2X:%2X.%u - Multimedia Audio Controller found.\n", pci.bus, pci.device, pci.function );

	// get I/O Port of audio controller
	module_es1370_port = (uint16_t) driver_pci_read( pci, DRIVER_PCI_REGISTER_bar0 ) & ~0b11;

	// retrieve interrupt number of network controller
	module_es1370_irq_number = (uint8_t) driver_pci_read( pci, DRIVER_PCI_REGISTER_irq );

	// debug
	// kernel -> log( (uint8_t *) "[ES1370] Port 0x%X, IRQ line %u\n", module_es1370_port, module_es1370_irq_number );

	// obtain current PCI configuration
	uint16_t command = (uint16_t) driver_pci_read( pci, DRIVER_PCI_REGISTER_status_and_command );

	// enable I/O Port access, Bus Master and Interrupts
	command |= DRIVER_PCI_REGISTER_CONTROL_IO_SPACE;
	command |= DRIVER_PCI_REGISTER_CONTROL_BUS_MASTER;
	command &= ~DRIVER_PCI_REGISTER_CONTROL_IRQ_DISABLE;

	// apply
	driver_pci_write( pci, DRIVER_PCI_REGISTER_status_and_command, command );

	// hold the door
	while( TRUE );
}