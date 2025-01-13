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
	#include	"./vio/config.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./vio/data.c"

void _entry( uintptr_t kernel_ptr ) {
	// preserve kernel structure pointer
	kernel = (struct KERNEL *) kernel_ptr;

	// check every bus;device;function of PCI controller
	for( uint16_t b = 0; b < 256; b++ )
		for( uint8_t d = 0; d < 32; d++ )
			for( uint8_t f = 0; f < 8; f++ ) {
				// PCI properties
				struct DRIVER_PCI_STRUCTURE pci = { EMPTY, b, d, f };

				// retrieved class-subclass
				uint32_t device_vendor = driver_pci_read( pci, DRIVER_PCI_REGISTER_vendor_and_device );

				if( device_vendor != STD_MAX_unsigned ) kernel -> log( (uint8_t *) "0x%4X, 0x%4X\n", (uint16_t) device_vendor, device_vendor >> STD_MOVE_WORD );
			}

	// infinite loop :)
	while( TRUE ) __asm__ volatile( "hlt" );
}
