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
	for( uint8_t t = 0; t < 5; t++ ) { driver_port_out_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_REGISTER_STRUCTURE, command ), 0x0004 ); kernel -> time_sleep( 11 ); driver_port_out_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_REGISTER_STRUCTURE, command ), 0x0000 ); }

	// command register contains default value?
	if( driver_port_in_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_REGISTER_STRUCTURE, command ) ) ) return FALSE;	// no

	// status register contains default value?
	if( driver_port_in_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_REGISTER_STRUCTURE, status ) ) != 0x0020 ) return FALSE;	// no

	// clear status register
	driver_port_out_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_REGISTER_STRUCTURE, status ), 0x00FF );

	// modify register contains default value?
	if( driver_port_in_byte( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_REGISTER_STRUCTURE, start_of_frame_modify ) ) != 0x40 ) return FALSE;	// no

	// check command register function
	driver_port_out_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_REGISTER_STRUCTURE, command ), 0x0002 ); kernel -> time_sleep( 42 );

	// test passed?
	if( driver_port_in_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_REGISTER_STRUCTURE, command ) ) & 0x0002 ) return FALSE;	// no

	// yes
	return TRUE;
}

uintptr_t driver_usb_queue_empty( void ) {
	// assign queue area
	struct DRIVER_USB_QUEUE_STRUCTURE *queue = (struct DRIVER_USB_QUEUE_STRUCTURE *) (kernel -> memory_alloc_page() | KERNEL_PAGE_logical);

	// fill with empty entries
	for( uint64_t i = 0; i < STD_PAGE_byte / sizeof( struct DRIVER_USB_QUEUE_STRUCTURE ); i++ ) {
		// next entry
		queue[ i ].head_link_pointer_and_flags = DRIVER_USB_DEFAULT_FLAG_terminate;

		// and current
		queue[ i ].element_link_pointer_and_flags = DRIVER_USB_DEFAULT_FLAG_terminate;
	}

	// return address of queue
	return (uintptr_t) queue;
}

void driver_usb_uhci_queue( uint32_t *frame_list ) {
	// acquire 1/1024u query
	driver_usb_queue_1ms = (struct DRIVER_USB_QUEUE_STRUCTURE *) driver_usb_queue_empty();

	// for each entry inside frame list
	for( uint64_t i = 0; i < STD_PAGE_byte >> STD_SHIFT_32; i++ )
		// insert ~1ms queue
		frame_list[ i ] = (uintptr_t) driver_usb_queue_1ms | DRIVER_USB_DEFAULT_FLAG_queue;
}

void driver_usb_descriptor( uint8_t type, uint8_t length, uintptr_t target ) {
	// prepare Transfer Descriptors area
	struct DRIVER_USB_TD_STRUCTURE *td = (struct DRIVER_USB_TD_STRUCTURE *) (kernel -> memory_alloc_page() | KERNEL_PAGE_logical);



}

uint8_t driver_usb_request_descriptor_length( void ) {
	// prepare Transfer Descriptors area
	struct DRIVER_USB_TD_STRUCTURE *td = (struct DRIVER_USB_TD_STRUCTURE *) (kernel -> memory_alloc_page() | KERNEL_PAGE_logical);

	// set REQUEST information
	uint64_t *buffer = (uint64_t *) &td[ 3 ];
	*buffer = DRIVER_USB_REQUEST_SETUP;

	//----------------------------------------------------------------------

	// SETUP descriptor
	td[ 0 ].pid = 0x2D;

	// if there was error while parsing this descriptor, decrease value
	td[ 0 ].error_counter = 3;

	// send 8 Bytes
	td[ 0 ].max_length = 0x07;

	// REQUEST information is located there ...
	td[ 0 ].buffer_pointer = (uintptr_t) &td[ 3 ];

	// even descriptor
	td[ 0 ].data_toggle = FALSE;

	// TD ready
	td[ 0 ].status = DRIVER_USB_TRANSFER_DESCRIPTOR_STATUS_active;

	// next TD
	td[ 0 ].flags = DRIVER_USB_DEFAULT_FLAG_data;
	td[ 0 ].link_pointer = (uintptr_t) &td[ 1 ] >> 4;	// which is at

	//----------------------------------------------------------------------

	// IN descriptor - inform device where to save requested data by SETUP descriptor
	td[ 1 ].pid = 0x69;

	// assume it is a Low Speed device
	td[ 1 ].high_speed = FALSE;

	// if there was error while parsing this descriptor, decrease value
	td[ 1 ].error_counter = 3;

	// receive 8 Bytes
	td[ 1 ].max_length = 0x07;

	// WRITE answer to this location ...
	td[ 1 ].buffer_pointer = (uintptr_t) &td[ 4 ];

	// odd descriptor
	td[ 1 ].data_toggle = TRUE;

	// TD ready
	td[ 1 ].status = DRIVER_USB_TRANSFER_DESCRIPTOR_STATUS_active;

	// next TD
	td[ 1 ].flags = DRIVER_USB_DEFAULT_FLAG_data;
	td[ 1 ].link_pointer = (uintptr_t) &td[ 2 ] >> 4;	// which is at

	//----------------------------------------------------------------------

	// STATUS descriptor - tell device that we are done with requests
	td[ 2 ].pid = 0xE1;

	// assume it is a Low Speed device
	td[ 2 ].high_speed = FALSE;

	// if there was error while parsing this descriptor, decrease value
	td[ 2 ].error_counter = 3;

	// we do not sending enything
	td[ 2 ].max_length = EMPTY;

	// last descriptor
	td[ 2 ].data_toggle = TRUE;

	// TD ready
	td[ 2 ].status = DRIVER_USB_TRANSFER_DESCRIPTOR_STATUS_active;

	// there will be no more descriptors
	td[ 2 ].flags = DRIVER_USB_DEFAULT_FLAG_terminate;

	//----------------------------------------------------------------------

	// insert Transfer Descriptors on Queue
	driver_usb_queue_1ms[ 0 ].element_link_pointer_and_flags = (uintptr_t) td;

	// wait for device
	volatile uint8_t *data = (uint8_t *) &td[ 4 ];
	while( ! *data );

	// retrieve full length of device descriptor in Bytes
	uint8_t answer = data[ 0 ];

	// remove Transfer Descriptors from Queue
	driver_usb_queue_1ms[ 0 ].element_link_pointer_and_flags = DRIVER_USB_DEFAULT_FLAG_terminate;

	// relase Transfer Descriptors
	kernel -> memory_release_page( (uintptr_t) td );

	// return answer
	return answer;
}

// void driver_usb_request_descriptor( uint8_t length, uintptr_t target ) {
// 	// prepare Transfer Descriptors area
// 	struct DRIVER_USB_TD_STRUCTURE *td = (struct DRIVER_USB_TD_STRUCTURE *) (kernel -> memory_alloc_page() | KERNEL_PAGE_logical);

// 	// set REQUEST information
// 	uint8_t *buffer = (uint8_t *) &td[ 5 ];
// 	buffer[ 0 ] = 0x80;
// 	buffer[ 1 ] = 0x06;
// 	buffer[ 3 ] = 0x01;
// 	buffer[ 6 ] = 0x08;

// 	//----------------------------------------------------------------------

// 	// SETUP descriptor
// 	td[ 0 ].pid = 0x2D;

// 	// assume it is a Low Speed device
// 	td[ 0 ].high_speed = FALSE;

// 	// if there was error while parsing this descriptor, decrease value
// 	td[ 0 ].error_counter = 3;

// 	// request N Bytes
// 	td[ 0 ].max_length = length - 1;

// 	// REQUEST information is located there ...
// 	td[ 0 ].buffer_pointer = (uintptr_t) &td[ 5 ];

// 	// TD ready
// 	td[ 0 ].status = DRIVER_USB_TRANSFER_DESCRIPTOR_STATUS_active;

// 	// next TD
// 	td[ 0 ].flags = DRIVER_USB_DEFAULT_FLAG_data;
// 	td[ 0 ].link_pointer = (uintptr_t) &td[ 1 ] >> 4;	// which is at

// 	//----------------------------------------------------------------------

// 	// IN descriptor - inform device where to save requested data by SETUP descriptor
// 	td[ 1 ].pid = 0x69;

// 	// assume it is a Low Speed device
// 	td[ 1 ].high_speed = FALSE;

// 	// if there was error while parsing this descriptor, decrease value
// 	td[ 1 ].error_counter = 3;

// 	// receive N Bytes
// 	td[ 1 ].max_length = 0x07;

// 	// WRITE answer to this location ...
// 	td[ 1 ].buffer_pointer = target;

// 	// odd descriptor
// 	td[ 1 ].data_toggle = TRUE;

// 	// TD ready
// 	td[ 1 ].status = DRIVER_USB_TRANSFER_DESCRIPTOR_STATUS_active;

// 	// next TD
// 	td[ 1 ].flags = DRIVER_USB_DEFAULT_FLAG_data;
// 	td[ 1 ].link_pointer = (uintptr_t) &td[ 2 ] >> 4;	// which is at

// 	//----------------------------------------------------------------------

// 	// IN descriptor - inform device where to save requested data by SETUP descriptor
// 	td[ 2 ].pid = 0x69;

// 	// assume it is a Low Speed device
// 	td[ 2 ].high_speed = FALSE;

// 	// if there was error while parsing this descriptor, decrease value
// 	td[ 2 ].error_counter = 3;

// 	// receive N Bytes
// 	td[ 2 ].max_length = 0x07;

// 	// WRITE answer to this location ...
// 	td[ 2 ].buffer_pointer = target + 0x08;

// 	// TD ready
// 	td[ 2 ].status = DRIVER_USB_TRANSFER_DESCRIPTOR_STATUS_active;

// 	// next TD
// 	td[ 2 ].flags = DRIVER_USB_DEFAULT_FLAG_data;
// 	td[ 2 ].link_pointer = (uintptr_t) &td[ 3 ] >> 4;	// which is at

// 	//----------------------------------------------------------------------

// 	// IN descriptor - inform device where to save requested data by SETUP descriptor
// 	td[ 3 ].pid = 0x69;

// 	// assume it is a Low Speed device
// 	td[ 3 ].high_speed = FALSE;

// 	// if there was error while parsing this descriptor, decrease value
// 	td[ 3 ].error_counter = 3;

// 	// receive N Bytes
// 	td[ 3 ].max_length = 0x02;

// 	// WRITE answer to this location ...
// 	td[ 3 ].buffer_pointer = target + 0x10;

// 	// odd descriptor
// 	td[ 3 ].data_toggle = TRUE;

// 	// TD ready
// 	td[ 3 ].status = DRIVER_USB_TRANSFER_DESCRIPTOR_STATUS_active;

// 	// next TD
// 	td[ 3 ].flags = DRIVER_USB_DEFAULT_FLAG_data;
// 	td[ 3 ].link_pointer = (uintptr_t) &td[ 4 ] >> 4;	// which is at

// 	//----------------------------------------------------------------------

// 	// STATUS descriptor - tell device that we are done with requests
// 	td[ 4 ].pid = 0xE1;

// 	// assume it is a Low Speed device
// 	td[ 4 ].high_speed = FALSE;

// 	// if there was error while parsing this descriptor, decrease value
// 	td[ 4 ].error_counter = 3;

// 	// we do not sending enything
// 	td[ 4 ].max_length = EMPTY;

// 	// last descriptor
// 	td[ 4 ].data_toggle = TRUE;

// 	// TD ready
// 	td[ 4 ].status = DRIVER_USB_TRANSFER_DESCRIPTOR_STATUS_active;

// 	// there will be no more descriptors
// 	td[ 4 ].flags = DRIVER_USB_DEFAULT_FLAG_terminate;

// 	//----------------------------------------------------------------------

// 	// insert Transfer Descriptors on Queue
// 	driver_usb_queue_1ms[ 0 ].element_link_pointer_and_flags = (uintptr_t) td;

// 	// wait for device
// 	volatile uint64_t *data = (uint64_t *) (target | KERNEL_PAGE_logical);
// 	while( ! *data );

// 	// retrieve full length of device descriptor in Bytes
// 	uint64_t answer = data[ 0 ];

// 	// remove Transfer Descriptors from Queue
// 	driver_usb_queue_1ms[ 0 ].element_link_pointer_and_flags = DRIVER_USB_DEFAULT_FLAG_terminate;

// 	// relase Transfer Descriptors
// 	kernel -> memory_release_page( (uintptr_t) td );
// }

uint8_t driver_usb_port_reset( uint8_t id ) {
	// by default, nothing there
	uint8_t connected = FALSE;

	// send command, reset
	driver_port_out_word( driver_usb_controller[ driver_usb_port[ id ].controller_id ].base_address + offsetof( struct DRIVER_USB_REGISTER_STRUCTURE, port[ driver_usb_port[ id ].port_id ] ), driver_port_in_word( driver_usb_controller[ driver_usb_port[ id ].controller_id ].base_address + offsetof( struct DRIVER_USB_REGISTER_STRUCTURE, port[ driver_usb_port[ id ].port_id ] ) ) | DRIVER_USB_PORT_STATUS_AND_CONTROL_port_reset ); kernel -> time_sleep( 50 );
	driver_port_out_word( driver_usb_controller[ driver_usb_port[ id ].controller_id ].base_address + offsetof( struct DRIVER_USB_REGISTER_STRUCTURE, port[ driver_usb_port[ id ].port_id ] ), driver_port_in_word( driver_usb_controller[ driver_usb_port[ id ].controller_id ].base_address + offsetof( struct DRIVER_USB_REGISTER_STRUCTURE, port[ driver_usb_port[ id ].port_id ] ) ) & ~DRIVER_USB_PORT_STATUS_AND_CONTROL_port_reset ); kernel -> time_sleep( 10 );

	// connection status
	for( uint8_t i = 0; i < 10; i++ ) {
		// retrieve port status
		uint16_t status = driver_port_in_word( driver_usb_controller[ driver_usb_port[ id ].controller_id ].base_address + offsetof( struct DRIVER_USB_REGISTER_STRUCTURE, port[ driver_usb_port[ id ].port_id ] ) );

		// device connected to port0?
		if( ! (status & DRIVER_USB_PORT_STATUS_AND_CONTROL_current_connect_status) ) {	// no
			// port disconnected
			kernel -> log( (uint8_t *) "[usb module].%u Port%u - disconnected.\n", driver_usb_port[ id ].controller_id, driver_usb_port[ id ].port_id );

			// ignore port
			break;
		}

		// port status change?
		if( ((status & DRIVER_USB_PORT_STATUS_AND_CONTROL_port_enable_change) || (status & DRIVER_USB_PORT_STATUS_AND_CONTROL_connect_status_change)) ) {
			// clean it
			driver_port_out_word( driver_usb_controller[ driver_usb_port[ id ].controller_id ].base_address + offsetof( struct DRIVER_USB_REGISTER_STRUCTURE, port[ driver_usb_port[ id ].port_id ] ), status & ~(DRIVER_USB_PORT_STATUS_AND_CONTROL_port_enable_change | DRIVER_USB_PORT_STATUS_AND_CONTROL_connect_status_change));

			// try again
			continue;
		}
		
		// port enabled?
		if( status & DRIVER_USB_PORT_STATUS_AND_CONTROL_port_enabled ) {
			// yes
			connected = TRUE;

			// continue
			break;
		}
		
		// try to enable port
		driver_port_out_word( driver_usb_controller[ driver_usb_port[ id ].controller_id ].base_address + offsetof( struct DRIVER_USB_REGISTER_STRUCTURE, port[ driver_usb_port[ id ].port_id ] ), status | DRIVER_USB_PORT_STATUS_AND_CONTROL_port_enabled );
	}

	// device connected?
	return connected;
}

void _entry( uintptr_t kernel_ptr ) {
	// preserve kernel structure pointer
	kernel = (struct KERNEL *) kernel_ptr;

	// assign space for discovered USB controllers
	driver_usb_controller = (struct DRIVER_USB_CONTROLLER_STRUCTURE *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct DRIVER_USB_CONTROLLER_STRUCTURE ) * DRIVER_USB_CONTROLLER_limit ) );

	// assign space for discovered USB ports
	driver_usb_port = (struct DRIVER_USB_PORT_STRUCTURE *) kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( sizeof( struct DRIVER_USB_PORT_STRUCTURE ) * DRIVER_USB_PORT_limit ) );

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
					kernel -> log( (uint8_t *) "[usb module].%u PCI %2X:%2X.%u - USB controller found. (Universal Serial Bus)\n", driver_usb_controller_count, pci.bus, pci.device, pci.function );

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
					driver_usb_controller[ driver_usb_controller_count ].size_byte = base_address_size;
					driver_usb_controller[ driver_usb_controller_count ].irq_line = line;

					// controller registered
					driver_usb_controller_count++;
				}
			}

	// detect controllers
	for( uint8_t i = 0; i < driver_usb_controller_count; i++ ) {
		// registered controller?
		if( driver_usb_controller[ i ].base_address ) {	// yes
			// check if UHCI controller
			if( driver_usb_detect_uhci( i ) ) {
				// show controller type
				kernel -> log( (uint8_t *) "[usb module].%u recognized as UHCI (Universal Host Controller Interface).\n", i );

				// configure UHCI controller

				// enable all type of interrupts
				driver_port_out_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_REGISTER_STRUCTURE, interrupt_enable ), 0x000F );

				// reset frame number
				driver_port_out_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_REGISTER_STRUCTURE, frame_number ), EMPTY );

				// alloc area for frame list
				driver_usb_controller[ i ].frame_base_address = kernel -> memory_alloc_page();
				driver_port_out_dword( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_REGISTER_STRUCTURE, frame_list_base_address ), driver_usb_controller[ i ].frame_base_address );

				// fill up frame list with queues
				driver_usb_uhci_queue( (uint32_t *) (driver_usb_controller[ i ].frame_base_address | KERNEL_PAGE_logical) );

				// clear controller status
				driver_port_out_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_REGISTER_STRUCTURE, status ), 0xFFFF );

				// start UHCI controller
				driver_port_out_word( driver_usb_controller[ i ].base_address + offsetof( struct DRIVER_USB_REGISTER_STRUCTURE, command ), TRUE | 1 << 6 );

				//-------------------

				// discover every port
				// for( uint16_t j = 0; j < driver_usb_controller[ i ].size_byte >> 4; j++ ) {	// thats a proper way of calculating amount of available ports inside controller, but for UHCI only
				for( uint16_t j = 0; j < 1; j++ ) {	// DEBUG
					// register port / it doesn't matter right now if device is connected to it or not
					driver_usb_port[ driver_usb_port_count ].flags = DRIVER_USB_PORT_FLAG_reserved;

					// preserve port and controller IDs
					driver_usb_port[ driver_usb_port_count ].controller_id = i;
					driver_usb_port[ driver_usb_port_count ].port_id = j;

					// port reset
					if( ! driver_usb_port_reset( driver_usb_port_count ) ) continue;	// device doesn't exist on port

					// device connected
					kernel -> log( (uint8_t *) "[usb module].%u Port%u - device connected.\n", i, j );

					// retrieve default descriptor from device
					struct DRIVER_USB_DESCRIPTOR_STANDARD *device_descriptor = (struct DRIVER_USB_DESCRIPTOR_STANDARD *) (kernel -> memory_alloc_page() | KERNEL_PAGE_logical);
					// driver_usb_descriptor( DRIVER_USB_REQUEST_SETUP, 0x08, (uintptr_t) device_descriptor & ~KERNEL_PAGE_logical );

					// device unplugged?
					// if( ! driver_usb_port_reset( driver_usb_port_count ) ) continue;	// yep

					// // alloc area for device full descriptor
					// uintptr_t device_descriptor = kernel -> memory_alloc( MACRO_PAGE_ALIGN_UP( length ) >> STD_SHIFT_PAGE ) & ~KERNEL_PAGE_logical;
					// driver_usb_request_descriptor( length, device_descriptor );	// request it
				}
			}
		}
	}

	// hold the door
	while( TRUE );
}