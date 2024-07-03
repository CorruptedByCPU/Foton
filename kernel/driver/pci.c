/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions of driver
	//----------------------------------------------------------------------
	#ifndef	DRIVER_PCI
		#include	"./pci.h"
	#endif

uint32_t driver_pci_read( struct DRIVER_PCI_STRUCTURE pci, uint32_t reg ) {
	// enable bit 31
	reg |= 1 << 31;

	// set bus number in bits 23..16
	reg |= pci.bus << 16;

	// set device number in bits 15..11
	reg |= pci.device << 11;

	// set function number in bits 10..8
	reg |= pci.function << 8;

	// send request
	driver_port_out_dword( DRIVER_PCI_PORT_command, reg );

	// return answer
	return driver_port_in_dword( DRIVER_PCI_PORT_data );
}

void driver_pci_write( struct DRIVER_PCI_STRUCTURE pci, uint32_t reg, uint32_t value ) {
	// enable bit 31
	reg |= 1 << 31;

	// set bus number in bits 23..16
	reg |= pci.bus << 16;

	// set device number in bits 15..11
	reg |= pci.device << 11;

	// set function number in bits 10..8
	reg |= pci.function << 8;

	// send request
	driver_port_out_dword( DRIVER_PCI_PORT_command, reg );

	// return answer
	driver_port_out_dword( DRIVER_PCI_PORT_data, value );
}

struct	DRIVER_PCI_STRUCTURE driver_pci_find_class_and_subclass( uint16_t class_and_subclass ) {
	// start from beginning of PCI address space
	struct DRIVER_PCI_STRUCTURE pci = { EMPTY };

	// check every bus;device;function of PCI controller
	for( pci.bus = 0; pci.bus < 32; pci.bus++ )
		for( pci.device = 0; pci.device < 32; pci.device++ )
			for( pci.function = 0; pci.function < 8; pci.function++ ) {
				// retrieve class and subclass
				pci.result = driver_pci_read( pci, DRIVER_PCI_REGISTER_class_and_subclass ) >> 16;

				// if found
				if( pci.result == class_and_subclass ) return pci;	// return PCI properties
			}

	// not found
	return (struct DRIVER_PCI_STRUCTURE) { EMPTY };
}