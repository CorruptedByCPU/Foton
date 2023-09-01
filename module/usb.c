/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions of kernel
	//----------------------------------------------------------------------
	#include	"../kernel/config.h"
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
	#include	"./usb/config.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./usb/data.c"

uint8_t driver_usb_detect_uhci( uint8_t i ) {
	// controller type: port?
	if( driver_usb_controller[ i ].type & DRIVER_USB_BASE_ADDRESS_type ) return FALSE;	// no

	// reset controller
	for( uint8_t t = 0; t < 5; t++ ) { driver_port_out_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_STRUCTURE_REGISTER, command ), 0x0004 ); kernel -> time_sleep( 11 ); driver_port_out_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_STRUCTURE_REGISTER, command ), 0x0000 ); }

	// command register contains default value?
	if( driver_port_in_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_STRUCTURE_REGISTER, command ) ) ) return FALSE;	// no

	// status register contains default value?
	if( driver_port_in_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_STRUCTURE_REGISTER, status ) ) != 0x0020 ) return FALSE;	// no

	// clear status register
	driver_port_out_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_STRUCTURE_REGISTER, status ), 0x00FF );

	// modify register contains default value?
	if( driver_port_in_byte( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_STRUCTURE_REGISTER, modify ) ) != 0x40 ) return FALSE;	// no

	// check command register function
	driver_port_out_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_STRUCTURE_REGISTER, command ), 0x0002 ); kernel -> time_sleep( 42 );

	// test passed?
	if( driver_port_in_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_STRUCTURE_REGISTER, command ) ) & 0x0002 ) return FALSE;	// no

	// yes
	return TRUE;
}

void _entry( uintptr_t kernel_ptr ) {
	// preserve kernel structure pointer
	kernel = (struct KERNEL *) kernel_ptr;

	// assign space for discovered USB controllers
	driver_usb_controller = (struct DRIVER_USB_CONTROLLER_STRUCTURE *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct DRIVER_USB_CONTROLLER_STRUCTURE ) * DRIVER_USB_CONTROLLER_limit ) );

	// amount of available USB controllers
	uint64_t driver_usb_controller_count = 0;

	// check every bus;device;function of PCI controller
	for( uint16_t b = 0; b < 256; b++ )
		for( uint8_t d = 0; d < 32; d++ )
			for( uint8_t f = 0; f < 8; f++ ) {
				// PCI properties
				struct DRIVER_PCI_STRUCTURE pci = { EMPTY, b, d, f };

				// if found
				if( (driver_pci_read( pci, DRIVER_PCI_REGISTER_class_and_subclass ) >> 16) == DRIVER_PCI_CLASS_SUBCLASS_usb ) {
					// choose IRQ line for controller
					uint8_t line = kernel -> io_apic_line_acquire();
					if( line ) {
						// update PCI line
						uint8_t config = (driver_pci_read( pci, DRIVER_PCI_REGISTER_irq ) & 0xFFFFFFF0) | line;
						driver_pci_write( pci, DRIVER_PCI_REGISTER_irq, config );
					} else continue;	// no available lines

					// show information about controller
					kernel -> log( (uint8_t *) "[usb module].%u PCI %2X:%2X.%u - Universal Serial Bus controller found.\n", driver_usb_controller_count, pci.bus, pci.device, pci.function );

					// try UHCI bar
					uint32_t hci = DRIVER_PCI_REGISTER_bar4;

					// type different than UHCI?
					uint64_t base_address_space = driver_pci_read( pci, hci );
					if( ! base_address_space )	// yes
						// change bar to EHCI/xHCI
						hci = DRIVER_PCI_REGISTER_bar0;
						base_address_space = driver_pci_read( pci, hci );

					// check type of base address space
					uint8_t *base_address_type = (uint8_t *) &driver_usb_string_memory;
					if( base_address_space & DRIVER_USB_BASE_ADDRESS_type ) base_address_type = (uint8_t *) &driver_usb_string_port;

					// get size of base address space
					driver_pci_write( pci, hci, 0xFFFFFFFF );
					uint32_t base_address_size = ~(driver_pci_read( pci, hci ) & ~1 ) + 1;
					// restore original value
					driver_pci_write( pci, hci, base_address_space );

					// retrieve base address space configuration and truncate
					uint8_t base_address_config = base_address_space & 0x0F; base_address_space &= ~0x0F;

					// base address space should be 64bit?
					if( base_address_config & 0b0100 ) base_address_space |= (uint64_t) driver_pci_read( pci, DRIVER_PCI_REGISTER_bar5 ) << 32;

					// show properties of device
					kernel -> log( (uint8_t *) "[usb module].%u  I/O %s at 0x%X [0x%X Bytes], I/O APIC line %u.\n", driver_usb_controller_count, base_address_type, base_address_space, base_address_size, line );

					// register USB controller
					driver_usb_controller[ driver_usb_controller_count ].type = base_address_space & DRIVER_USB_BASE_ADDRESS_type;
					driver_usb_controller[ driver_usb_controller_count ].base_address = base_address_space;
					driver_usb_controller[ driver_usb_controller_count ].irq_line = line;

					// controller registered
					driver_usb_controller_count++;
				}
			}

	// detect controllers
	for( uint8_t i = 0; i < DRIVER_USB_CONTROLLER_limit; i++ ) {
		// registered controller?
		if( driver_usb_controller[ i ].base_address ) {	// yes
			// check if UHCI controller
			if( driver_usb_detect_uhci( i ) ) {
				// show controller type
				kernel -> log( (uint8_t *) "[usb module].%u recognized as UHCI (Universal Host Controller Interface).\n", i );

				// configure UHCI controller

				// enable all type of interrupts
				driver_port_out_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_STRUCTURE_REGISTER, interrupt ), 0x000F );

				// reset frame number
				driver_port_out_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_STRUCTURE_REGISTER, frame_number ), EMPTY );

				// alloc area for frame list
				driver_usb_controller[ i ].frame_base_address = kernel -> memory_alloc( 1 );
				driver_port_out_dword( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_STRUCTURE_REGISTER, frame_base_address ), driver_usb_controller[ i ].frame_base_address & ~KERNEL_PAGE_logical );

				// clear controller status
				driver_port_out_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_STRUCTURE_REGISTER, status ), 0xFFFF );

				
				MACRO_DEBUF();

				// reset port0
				driver_port_out_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_STRUCTURE_REGISTER, port0 ), driver_port_in_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_STRUCTURE_REGISTER, port0 ) ) | (1 << 9) ); kernel -> time_sleep( 50 );
				driver_port_out_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_STRUCTURE_REGISTER, port0 ), driver_port_in_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_STRUCTURE_REGISTER, port0 ) ) & ~(1 << 9) ); kernel -> time_sleep( 10 );
			}
		}
	}

	// hold the door
	while( TRUE );
}