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

uint8_t module_usb_detect_uhci( uint8_t i ) {
	// controller type: port?
	if( module_usb_controller[ i ].type & MODULE_USB_BASE_ADDRESS_type ) return FALSE;	// no

	// reset controller
	for( uint8_t t = 0; t < 5; t++ ) { driver_port_out_word( module_usb_controller[ i ].base_address + offsetof( struct MODULE_USB_STRUCTURE_REGISTER, command ), 0x0004 ); kernel -> time_sleep( 16 ); driver_port_out_word( module_usb_controller[ i ].base_address + offsetof( struct MODULE_USB_STRUCTURE_REGISTER, command ), 0x0000 ); }

	// command register contains default value?
	if( driver_port_in_word( module_usb_controller[ i ].base_address + offsetof( struct MODULE_USB_STRUCTURE_REGISTER, command ) ) ) return FALSE;	// no

	// status register contains default value?
	if( driver_port_in_word( module_usb_controller[ i ].base_address + offsetof( struct MODULE_USB_STRUCTURE_REGISTER, status ) ) != 0x0020 ) return FALSE;	// no

	// clear status register
	driver_port_out_word( module_usb_controller[ i ].base_address + offsetof( struct MODULE_USB_STRUCTURE_REGISTER, status ), 0x00FF );

	// modify register contains default value?
	if( driver_port_in_byte( module_usb_controller[ i ].base_address + offsetof( struct MODULE_USB_STRUCTURE_REGISTER, start_of_frame_modify ) ) != 0x40 ) return FALSE;	// no

	// check command register function
	driver_port_out_word( module_usb_controller[ i ].base_address + offsetof( struct MODULE_USB_STRUCTURE_REGISTER, command ), 0x0002 ); kernel -> time_sleep( 64 );

	// test passed?
	if( driver_port_in_word( module_usb_controller[ i ].base_address + offsetof( struct MODULE_USB_STRUCTURE_REGISTER, command ) ) & 0x0002 ) return FALSE;	// no

	// yes
	return TRUE;
}

uintptr_t module_usb_queue_empty( void ) {
	// assign queue area
	struct MODULE_USB_STRUCTURE_QUEUE *queue = (struct MODULE_USB_STRUCTURE_QUEUE *) kernel -> memory_alloc_low( TRUE );

	// fill with empty entries
	for( uint64_t i = 0; i < STD_PAGE_byte / sizeof( struct MODULE_USB_STRUCTURE_QUEUE ); i++ ) {
		// next entry
		queue[ i ].head_link_pointer_and_flags = MODULE_USB_DEFAULT_FLAG_terminate;

		// and current
		queue[ i ].element_link_pointer_and_flags = MODULE_USB_DEFAULT_FLAG_terminate;
	}

	// return address of queue
	return (uintptr_t) queue;
}

void module_usb_uhci_queue( uint32_t *frame_list ) {
	// acquire 1/1024u query
	module_usb_queue_1ms = (struct MODULE_USB_STRUCTURE_QUEUE *) module_usb_queue_empty();

	// for each entry inside frame list
	for( uint64_t i = 0; i < STD_PAGE_byte >> STD_SHIFT_32; i++ )
		// insert ~1ms queue
		frame_list[ i ] = (uintptr_t) module_usb_queue_1ms | MODULE_USB_DEFAULT_FLAG_queue;
}

void module_usb_debug_descriptors( struct MODULE_USB_STRUCTURE_TD *td ) {
	kernel -> log( (uint8_t *) "Link pointer 0x%X (flags: %4b)\n", td -> link_pointer << 4, td -> flags );
	kernel -> log( (uint8_t *) "Status: %8b (", td -> status );
	uint16_t status = td -> status;
	if( status & 1 << 7 ) { kernel -> log( (uint8_t *) "Active" ); status ^= 1 << 7; if( status ) kernel -> log( (uint8_t *) ", " ); }
	if( status & 1 << 6 ) { kernel -> log( (uint8_t *) "Stalled" ); status ^= 1 << 6; if( status ) kernel -> log( (uint8_t *) ", " ); }
	if( status & 1 << 5 ) { kernel -> log( (uint8_t *) "Data Buffer Error" ); status ^= 1 << 5; if( status ) kernel -> log( (uint8_t *) ", " ); }
	if( status & 1 << 4 ) { kernel -> log( (uint8_t *) "Babble Detected" ); status ^= 1 << 4; if( status ) kernel -> log( (uint8_t *) ", " ); }
	if( status & 1 << 3 ) { kernel -> log( (uint8_t *) "NAK Received" ); status ^= 1 << 3; if( status ) kernel -> log( (uint8_t *) ", " ); }
	if( status & 1 << 2 ) { kernel -> log( (uint8_t *) "CRC/Time Out Error" ); status ^= 1 << 2; if( status ) kernel -> log( (uint8_t *) ", " ); }
	if( status & 1 << 1 ) { kernel -> log( (uint8_t *) "Bitstuff Error" ); status ^= 1 << 1; if( status ) kernel -> log( (uint8_t *) ", " ); }
	kernel -> log( (uint8_t *) ")\n" );
	kernel -> log( (uint8_t *) "Error Counter: %u, Device Address: %u, Max Length: ", td -> error_counter, td -> device_address );
	if( ! td -> max_length || td -> max_length == 2047 ) kernel -> log( (uint8_t *) "0\n" );
	else kernel -> log( (uint8_t *) "%u\n", td -> max_length + 1 );
}

// void module_usb_descriptor( uint8_t port, uint8_t type, uint8_t length, uintptr_t target ) {
// 	// prepare Transfer Descriptors area
// 	struct MODULE_USB_STRUCTURE_TD *td = (struct MODULE_USB_STRUCTURE_TD *) kernel -> memory_alloc_low( TRUE );

// 	// amount of required TDs
// 	uint64_t tds = length / module_usb_port[ port ].max_packet_size;
// 	if( length % module_usb_port[ port ].max_packet_size ) tds++;

// 	uint64_t *buffer = (uint64_t *) &td[ tds + 2 ];
// 	switch( type ) {
// 		case 1: {
// 			// set REQUEST information
// 			*buffer = 0x0000000001000680 | ((uint64_t) length << 48);

// 			break;
// 		}

// 		case 2: {
// 			// set ADDRESS
// 			*buffer = 0x0000000000000500 | (((uint64_t) ((uintptr_t) &module_usb_port[ module_usb_port_count ] - (uintptr_t) module_usb_port) / sizeof( struct MODULE_USB_STRUCTURE_PORT )) << 16);

// 			break;
// 		}
// 	}

// 	uint8_t i = 0;

// 	uint8_t status;

// 	//----------------------------------------------------------------------

// 	// SETUP descriptor
// 	td[ i ].pid = 0x2D;

// 	// send this descriptor to
// 	td[ i ].device_address = module_usb_port[ port ].address_id;

// 	// set device speed
// 	td[ i ].low_speed = module_usb_port[ port ].low_speed;

// 	// if there was error while parsing this descriptor, decrease value
// 	td[ i ].error_counter = 3;

// 	// request N Bytes
// 	td[ i ].max_length = length - 1; if( type == 2 ) td[ i ].max_length = 7; else if( ! length ) td[ i ].max_length = 0b11111110100;

// 	// REQUEST description is located there ...
// 	if( length ) td[ i ].buffer_pointer = (uintptr_t) &td[ tds + 2 ];

// 	// even descriptor
// 	td[ i ].data_toggle = FALSE;

// 	// TD ready
// 	td[ i ].status = MODULE_USB_TRANSFER_DESCRIPTOR_STATUS_active;

// 	// next TD
// 	td[ i ].flags = MODULE_USB_DEFAULT_FLAG_data;
// 	td[ i ].link_pointer = (uintptr_t) &td[ i + 1 ] >> 4;	// which is at

// 	// debug
// 	kernel -> log( (uint8_t *) "\n\n>> SEND\n" );
// 	module_usb_debug_descriptors( (struct MODULE_USB_STRUCTURE_TD *) &td[ i ] );

// 	//----------------------------------------------------------------------

// 	uint8_t toggle = TRUE;
// 	if( length ) { i++; for( ; i < 254; i++ ) {
// 		// IN descriptor - inform device where to save requested data by SETUP descriptor
// 		td[ i ].pid = 0x69;

// 		// send this descriptor to
// 		td[ i ].device_address = module_usb_port[ port ].address_id;

// 		// set device speed
// 		td[ i ].low_speed = module_usb_port[ port ].low_speed;

// 		// if there was error while parsing this descriptor, decrease value
// 		td[ i ].error_counter = 3;

// 		// receive chunk of Bytes
// 		if( length > module_usb_port[ port ].max_packet_size ) {
// 			td[ i ].max_length = module_usb_port[ port ].max_packet_size - 1;
// 			length -= module_usb_port[ port ].max_packet_size;
// 		} else {
// 			td[ i ].max_length = length - 1;
// 			length = EMPTY;
// 		}

// 		// WRITE answer to this location ...
// 		td[ i ].buffer_pointer = target + ((i - 1) * module_usb_port[ port ].max_packet_size);

// 		// odd descriptor
// 		td[ i ].data_toggle = toggle;

// 		// TD ready
// 		td[ i ].status = MODULE_USB_TRANSFER_DESCRIPTOR_STATUS_active;

// 		// next TD
// 		td[ i ].flags = MODULE_USB_DEFAULT_FLAG_data;
// 		td[ i ].link_pointer = (uintptr_t) &td[ i + 1 ] >> 4;	// which is at

// 		// debug
// 		module_usb_debug_descriptors( (struct MODULE_USB_STRUCTURE_TD *) &td[ i ] );

// 		if( ! length ) break;

// 		if( toggle ) toggle = FALSE; else toggle = TRUE;
// 	} }

// 	//----------------------------------------------------------------------

// 	// STATUS descriptor - tell device that we are done with requests
// 	td[ ++i ].pid = 0xE1;

// 	// send this descriptor to
// 	td[ i ].device_address = module_usb_port[ port ].address_id;

// 	// set device speed
// 	td[ i ].low_speed = module_usb_port[ port ].low_speed;

// 	// if there was error while parsing this descriptor, decrease value
// 	td[ i ].error_counter = 3;

// 	// we do not sending enything
// 	td[ i ].max_length = 0b11111110100;

// 	// last descriptor
// 	td[ i ].data_toggle = TRUE;

// 	// TD ready
// 	td[ i ].status = MODULE_USB_TRANSFER_DESCRIPTOR_STATUS_active;

// 	// there will be no more descriptors
// 	td[ i ].flags = MODULE_USB_DEFAULT_FLAG_terminate;

// 	// debug
// 	module_usb_debug_descriptors( (struct MODULE_USB_STRUCTURE_TD *) &td[ i ] );

// 	//----------------------------------------------------------------------

// 	// insert Transfer Descriptors on Queue
// 	module_usb_queue_1ms[ 0 ].element_link_pointer_and_flags = (uintptr_t) td;

// 	// wait for device
// 	while( (td[ 0 ].status & 0b10000000) || (td[ tds ].status & 0b10000000) ) {
// 		__asm__ volatile( "nop" );
// 	}

// 	// debug
// 	kernel -> log( (uint8_t *) "<< RECEIVED\n" );
// 	for( uint64_t i = 0; i < 2 + tds; i++ ) module_usb_debug_descriptors( (struct MODULE_USB_STRUCTURE_TD *) &td[ i ] );

// 	// remove Transfer Descriptors from Queue
// 	module_usb_queue_1ms[ 0 ].element_link_pointer_and_flags = MODULE_USB_DEFAULT_FLAG_terminate;

// 	// relase Transfer Descriptors
// 	kernel -> memory_release( (uintptr_t) td, TRUE );
// }

void module_usb_descriptor( uint8_t port, uint8_t type, uint8_t length, uintptr_t target ) {
	// prepare Transfer Descriptors area
	struct MODULE_USB_STRUCTURE_TD *td = (struct MODULE_USB_STRUCTURE_TD *) kernel -> memory_alloc_low( TRUE );
	uint32_t *data = (uint32_t *) td;

	switch( type ) {
		// SETUP packet
		case 0: {
			td[ 0 ].flags = 0b0100;
			td[ 0 ].link_pointer = (uintptr_t) &td[ 1 ] >> 4;
			data[ 1 ] = 0x18800000; td[ 0 ].low_speed = module_usb_port[ port ].low_speed;
			data[ 2 ] = 0x0000002D; td[ 0 ].max_length = 7; td[ 0 ].data_toggle = FALSE;
			uint64_t setup = (uint64_t) &td[ 3 ]; data[ 3 ] = setup;

			td[ 1 ].flags = 0b0100;
			td[ 1 ].link_pointer = (uintptr_t) &td[ 2 ] >> 4;
			data[ 9 ] = 0x18800000; td[ 1 ].low_speed = module_usb_port[ port ].low_speed;
			data[ 10 ] = 0x00000069; td[ 1 ].max_length = 7; td[ 1 ].data_toggle = TRUE;
			data[ 11 ] = target;

			td[ 2 ].flags = 0b0001;
			td[ 2 ].link_pointer = EMPTY;
			data[ 17 ] = 0x19800000; td[ 2 ].low_speed = module_usb_port[ port ].low_speed;
			data[ 18 ] = 0x000000E1; td[ 2 ].max_length = -1; td[ 2 ].data_toggle = TRUE;
			data[ 19 ] = EMPTY;

			data[ 24 ] = 0x01000680;
			data[ 25 ] = 0x00080000;

			break;
		}

		// ADDRESS packet
		case 1: {
			td[ 0 ].flags = 0b0100;
			td[ 0 ].link_pointer = (uintptr_t) &td[ 1 ] >> 4;
			data[ 1 ] = 0x18800000; td[ 0 ].low_speed = module_usb_port[ port ].low_speed;
			data[ 2 ] = 0x0000002D; td[ 0 ].max_length = 7; td[ 0 ].data_toggle = FALSE;
			uint64_t setup = (uint64_t) &td[ 2 ]; data[ 3 ] = setup;

			td[ 1 ].flags = 0b0001;
			td[ 1 ].link_pointer = EMPTY;
			data[ 9 ] = 0x19800000; td[ 1 ].low_speed = module_usb_port[ port ].low_speed;
			data[ 10 ] = 0x00000069; td[ 1 ].max_length = -1; td[ 1 ].data_toggle = TRUE;
			data[ 11 ] = EMPTY;

			data[ 16 ] = 0x00000500 | ((uint64_t) ((uintptr_t) &module_usb_port[ module_usb_port_count ] - (uintptr_t) module_usb_port) / sizeof( struct MODULE_USB_STRUCTURE_PORT )) << 16;

			break;
		}

		// DEFAULT packet
		case 2: {
			td[ 0 ].flags = 0b0100;
			td[ 0 ].link_pointer = (uintptr_t) &td[ 1 ] >> 4;
			data[ 1 ] = 0x18800000; td[ 0 ].low_speed = module_usb_port[ port ].low_speed;
			data[ 2 ] = 0x0000002D; td[ 0 ].max_length = 7; td[ 0 ].data_toggle = FALSE; td[ 0 ].device_address = module_usb_port[ module_usb_port_count ].address_id;
			uint64_t setup = (uint64_t) &td[ 5 ]; data[ 3 ] = setup;

			// ----------
			if( module_usb_port[ port ].low_speed ) {
				td[ 1 ].flags = 0b0100;
				td[ 1 ].link_pointer = (uintptr_t) &td[ 2 ] >> 4;
				data[ 9 ] = 0x18800000; td[ 1 ].low_speed = module_usb_port[ port ].low_speed;
				data[ 10 ] = 0x00000069; td[ 1 ].max_length = 7; td[ 1 ].data_toggle = TRUE; td[ 1 ].device_address = module_usb_port[ module_usb_port_count ].address_id;
				data[ 11 ] = target;

				td[ 2 ].flags = 0b0100;
				td[ 2 ].link_pointer = (uintptr_t) &td[ 3 ] >> 4;
				data[ 17 ] = 0x18800000; td[ 2 ].low_speed = module_usb_port[ port ].low_speed;
				data[ 18 ] = 0x00000069; td[ 2 ].max_length = 7; td[ 2 ].data_toggle = FALSE; td[ 2 ].device_address = module_usb_port[ module_usb_port_count ].address_id;
				data[ 19 ] = target + 0x08;

				td[ 3 ].flags = 0b0100;
				td[ 3 ].link_pointer = (uintptr_t) &td[ 4 ] >> 4;
				data[ 25 ] = 0x18800000; td[ 3 ].low_speed = module_usb_port[ port ].low_speed;
				data[ 26 ] = 0x00000069; td[ 3 ].max_length = 1; td[ 3 ].data_toggle = TRUE; td[ 3 ].device_address = module_usb_port[ module_usb_port_count ].address_id;
				data[ 27 ] = target + 0x10;
			} else {
				td[ 1 ].flags = 0b0100;
				td[ 1 ].link_pointer = (uintptr_t) &td[ 4 ] >> 4;
				data[ 9 ] = 0x18800000; td[ 1 ].low_speed = module_usb_port[ port ].low_speed;
				data[ 10 ] = 0x00000069; td[ 1 ].max_length = 17; td[ 1 ].data_toggle = TRUE; td[ 1 ].device_address = module_usb_port[ module_usb_port_count ].address_id;
				data[ 11 ] = target;
			}
	
			td[ 4 ].flags = 0b0001;
			td[ 4 ].link_pointer = EMPTY;
			data[ 33 ] = 0x19800000; td[ 4 ].low_speed = module_usb_port[ port ].low_speed;
			data[ 34 ] = 0x000000E1; td[ 4 ].max_length = -1; td[ 4 ].data_toggle = TRUE; td[ 4 ].device_address = module_usb_port[ module_usb_port_count ].address_id;
			data[ 35 ] = EMPTY;

			data[ 40 ] = 0x01000680;
			data[ 41 ] = 0x00000000 | 0x12 << 16;

			break;

			// td[ 0 ].flags = 0b0100;
			// td[ 0 ].link_pointer = (uintptr_t) &td[ 1 ] >> 4;
			// data[ 1 ] = 0x18800000; td[ 0 ].low_speed = module_usb_port[ port ].low_speed;
			// data[ 2 ] = 0x0000002D; td[ 0 ].max_length = 7; td[ 0 ].data_toggle = FALSE; td[ 0 ].device_address = module_usb_port[ module_usb_port_count ].address_id;
			// uint64_t setup = (uint64_t) &td[ 5 ]; data[ 3 ] = setup;

			// td[ 1 ].flags = 0b0100;
			// td[ 1 ].link_pointer = (uintptr_t) &td[ 4 ] >> 4;
			// data[ 9 ] = 0x18800000; td[ 1 ].low_speed = module_usb_port[ port ].low_speed;
			// data[ 10 ] = 0x00000069; td[ 1 ].max_length = 63; td[ 1 ].data_toggle = TRUE; td[ 1 ].device_address = module_usb_port[ module_usb_port_count ].address_id;
			// data[ 11 ] = target;

			// // td[ 2 ].flags = 0b0100;
			// // td[ 2 ].link_pointer = (uintptr_t) &td[ 3 ] >> 4;
			// // data[ 17 ] = 0x18800000; td[ 2 ].low_speed = module_usb_port[ port ].low_speed;
			// // data[ 18 ] = 0x00000069; td[ 2 ].max_length = 7; td[ 2 ].data_toggle = FALSE; td[ 2 ].device_address = module_usb_port[ module_usb_port_count ].address_id;
			// // data[ 19 ] = target + 0x08;

			// // td[ 3 ].flags = 0b0100;
			// // td[ 3 ].link_pointer = (uintptr_t) &td[ 4 ] >> 4;
			// // data[ 25 ] = 0x18800000; td[ 3 ].low_speed = module_usb_port[ port ].low_speed;
			// // data[ 26 ] = 0x00000069; td[ 3 ].max_length = 1; td[ 3 ].data_toggle = TRUE; td[ 3 ].device_address = module_usb_port[ module_usb_port_count ].address_id;
			// // data[ 27 ] = target + 0x10;

			// td[ 4 ].flags = 0b0001;
			// td[ 4 ].link_pointer = EMPTY;
			// data[ 33 ] = 0x19800000; td[ 4 ].low_speed = module_usb_port[ port ].low_speed;
			// data[ 34 ] = 0x000000E1; td[ 4 ].max_length = -1; td[ 4 ].data_toggle = TRUE; td[ 4 ].device_address = module_usb_port[ module_usb_port_count ].address_id;
			// data[ 35 ] = EMPTY;

			// data[ 40 ] = 0x01000680;
			// data[ 41 ] = 0x00000000 | 0x12 << 16;

			// break;
		}
	}

	//----------------------------------------------------------------------

	// insert Transfer Descriptors on Queue
	module_usb_queue_1ms[ 0 ].element_link_pointer_and_flags = (uintptr_t) td;

	// wait for device
	while( (td[ 0 ].status & 0b10000000) || (td[ 1 ].status & 0b10000000) ) { __asm__ volatile( "nop" ); }	// i don't know why... but clang removed whole line if there is no at least something like "nop"

	// remove Transfer Descriptors from Queue
	module_usb_queue_1ms[ 0 ].element_link_pointer_and_flags = MODULE_USB_DEFAULT_FLAG_terminate;

	// relase Transfer Descriptors
	kernel -> memory_release( (uintptr_t) td, TRUE );
}

uint16_t module_usb_port_reset( uint8_t id ) {
	// by default, nothing there
	uint8_t connected = FALSE;
	uint16_t status = EMPTY;

	// send command, reset
	driver_port_out_word( module_usb_controller[ module_usb_port[ id ].controller_id ].base_address + offsetof( struct MODULE_USB_STRUCTURE_REGISTER, port[ module_usb_port[ id ].port_id ] ), driver_port_in_word( module_usb_controller[ module_usb_port[ id ].controller_id ].base_address + offsetof( struct MODULE_USB_STRUCTURE_REGISTER, port[ module_usb_port[ id ].port_id ] ) ) & ~MODULE_USB_PORT_STATUS_AND_CONTROL_port_reset ); kernel -> time_sleep( 64 );
	driver_port_out_word( module_usb_controller[ module_usb_port[ id ].controller_id ].base_address + offsetof( struct MODULE_USB_STRUCTURE_REGISTER, port[ module_usb_port[ id ].port_id ] ), driver_port_in_word( module_usb_controller[ module_usb_port[ id ].controller_id ].base_address + offsetof( struct MODULE_USB_STRUCTURE_REGISTER, port[ module_usb_port[ id ].port_id ] ) ) & ~MODULE_USB_PORT_STATUS_AND_CONTROL_port_reset ); kernel -> time_sleep( 16 );

	// connection status
	for( uint8_t i = 0; i < 10; i++ ) {
		// retrieve port status
		status = driver_port_in_word( module_usb_controller[ module_usb_port[ id ].controller_id ].base_address + offsetof( struct MODULE_USB_STRUCTURE_REGISTER, port[ module_usb_port[ id ].port_id ] ) );

		// device connected to port0?
		if( ! (status & MODULE_USB_PORT_STATUS_AND_CONTROL_current_connect_status) ) {	// no
			// debug
			kernel -> log( (uint8_t *) "[USB].%u Port%u - disconnected.\n", module_usb_port[ id ].controller_id, module_usb_port[ id ].port_id );

			// ignore port
			break;
		}

		// port status change?
		if( ((status & MODULE_USB_PORT_STATUS_AND_CONTROL_port_enable_change) || (status & MODULE_USB_PORT_STATUS_AND_CONTROL_connect_status_change)) ) {
			// clean it
			driver_port_out_word( module_usb_controller[ module_usb_port[ id ].controller_id ].base_address + offsetof( struct MODULE_USB_STRUCTURE_REGISTER, port[ module_usb_port[ id ].port_id ] ), status & ~(MODULE_USB_PORT_STATUS_AND_CONTROL_port_enable_change | MODULE_USB_PORT_STATUS_AND_CONTROL_connect_status_change));

			// try again
			continue;
		}
		
		// port enabled?
		if( status & MODULE_USB_PORT_STATUS_AND_CONTROL_port_enabled ) {
			// yes
			connected = TRUE;

			// continue
			break;
		}
		
		// try to enable port
		driver_port_out_word( module_usb_controller[ module_usb_port[ id ].controller_id ].base_address + offsetof( struct MODULE_USB_STRUCTURE_REGISTER, port[ module_usb_port[ id ].port_id ] ), status | MODULE_USB_PORT_STATUS_AND_CONTROL_port_enabled );
	}

	// device connected?
	if( connected ) return status;
	else return EMPTY;
}

void _entry( uintptr_t kernel_ptr ) {
	// preserve kernel structure pointer
	kernel = (struct KERNEL *) kernel_ptr;

	// assign space for discovered USB controllers
	module_usb_controller = (struct MODULE_USB_STRUCTURE_CONTROLLER *) kernel -> memory_alloc_low( MACRO_PAGE_ALIGN_UP( sizeof( struct MODULE_USB_STRUCTURE_CONTROLLER ) * MODULE_USB_CONTROLLER_limit ) >> STD_SHIFT_PAGE );

	// assign space for discovered USB ports
	module_usb_port = (struct MODULE_USB_STRUCTURE_PORT *) kernel -> memory_alloc_low( MACRO_PAGE_ALIGN_UP( sizeof( struct MODULE_USB_STRUCTURE_PORT ) * MODULE_USB_PORT_limit ) >> STD_SHIFT_PAGE );

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

					// debug
					kernel -> log( (uint8_t *) "[USB].%u PCI %2X:%2X.%u - USB controller found. (Universal Serial Bus)\n", module_usb_controller_count, pci.bus, pci.device, pci.function );

					// try UHCI bar
					uint32_t hci = DRIVER_PCI_REGISTER_bar4;

					// type different than UHCI?
					uint64_t base_address_space = driver_pci_read( pci, hci );
					if( ! base_address_space )	// yes
						// change bar to EHCI/xHCI
						hci = DRIVER_PCI_REGISTER_bar0;
						base_address_space = driver_pci_read( pci, hci );

					// check type of base address space
					uint8_t *base_address_type = (uint8_t *) &module_usb_string_memory;
					if( base_address_space & MODULE_USB_BASE_ADDRESS_type ) base_address_type = (uint8_t *) &module_usb_string_port;

					// get size of base address space
					driver_pci_write( pci, hci, 0xFFFFFFFF );
					uint32_t base_address_size = ~(driver_pci_read( pci, hci ) & ~1 ) + 1;
					// restore original value
					driver_pci_write( pci, hci, base_address_space );

					// retrieve base address space configuration and truncate
					uint8_t base_address_config = base_address_space & 0x0F; base_address_space &= ~0x0F;

					// base address space should be 64bit?
					if( base_address_config & 0b0100 ) base_address_space |= (uint64_t) driver_pci_read( pci, DRIVER_PCI_REGISTER_bar5 ) << 32;

					// debug
					kernel -> log( (uint8_t *) "[USB].%u I/O %s at 0x%X [0x%X Bytes], I/O APIC line %u.\n", module_usb_controller_count, base_address_type, base_address_space, base_address_size, line );

					// register USB controller
					module_usb_controller[ module_usb_controller_count ].type = base_address_space & MODULE_USB_BASE_ADDRESS_type;
					module_usb_controller[ module_usb_controller_count ].base_address = base_address_space;
					module_usb_controller[ module_usb_controller_count ].size_byte = base_address_size;
					module_usb_controller[ module_usb_controller_count ].irq_line = line;

					// controller registered
					module_usb_controller_count++;
				}
			}

	// detect controllers
	for( uint8_t i = 0; i < module_usb_controller_count; i++ ) {
		// registered controller?
		if( module_usb_controller[ i ].base_address ) {	// yes
			// check if UHCI controller
			if( module_usb_detect_uhci( i ) ) {
				// debug
				kernel -> log( (uint8_t *) "[USB].%u recognized as UHCI (Universal Host Controller Interface).\n", i );

				// configure UHCI controller

				// enable all type of interrupts
				driver_port_out_word( module_usb_controller[ i ].base_address + offsetof( struct MODULE_USB_STRUCTURE_REGISTER, interrupt_enable ), 0x000F );

				// reset frame number
				driver_port_out_word( module_usb_controller[ i ].base_address + offsetof( struct MODULE_USB_STRUCTURE_REGISTER, frame_number ), EMPTY );

				// alloc area for frame list
				module_usb_controller[ i ].frame_base_address = kernel -> memory_alloc_low( TRUE ) & ~KERNEL_PAGE_mirror;
				driver_port_out_dword( module_usb_controller[ i ].base_address + offsetof( struct MODULE_USB_STRUCTURE_REGISTER, frame_list_base_address ), module_usb_controller[ i ].frame_base_address );

				// fill up frame list with queues
				module_usb_uhci_queue( (uint32_t *) (module_usb_controller[ i ].frame_base_address | KERNEL_PAGE_mirror) );

				// clear controller status
				driver_port_out_word( module_usb_controller[ i ].base_address + offsetof( struct MODULE_USB_STRUCTURE_REGISTER, status ), 0xFFFF );

				// start UHCI controller
				driver_port_out_word( module_usb_controller[ i ].base_address + offsetof( struct MODULE_USB_STRUCTURE_REGISTER, command ), TRUE | 1 << 6 );

				//-------------------

				// discover every port
				for( uint16_t j = 0; j < module_usb_controller[ i ].size_byte >> 4; j++ ) {	// thats a proper way of calculating amount of available ports inside controller, but for UHCI only
				// for( uint16_t j = 0; j < 1; j++ ) {	// DEBUG
					// register port / it doesn't matter right now if device is connected to it or not
					module_usb_port[ module_usb_port_count ].flags = MODULE_USB_PORT_FLAG_reserved;

					// preserve port and controller IDs
					module_usb_port[ module_usb_port_count ].controller_id = i;
					module_usb_port[ module_usb_port_count ].port_id = j;
					module_usb_port[ module_usb_port_count ].max_packet_size = 0x08;	// default 8 Bytes

					// port reset
					uint16_t status = EMPTY;
					if( ! (status = module_usb_port_reset( module_usb_port_count )) ) continue;	// device doesn't exist on port

					// register device speed
					module_usb_port[ module_usb_port_count ].low_speed = status >> 8;

					// debug
					kernel -> log( (uint8_t *) "[USB].%u Port%u - device connected.\n", i, j );

					// retrieve default descriptor from device
					struct MODULE_USB_DESCRIPTOR_STANDARD *device_descriptor = (struct MODULE_USB_DESCRIPTOR_STANDARD *) kernel -> memory_alloc_low( TRUE );
					module_usb_descriptor( module_usb_port_count, 0, 0x08, (uintptr_t) device_descriptor & ~KERNEL_PAGE_mirror );

					// port reset
					if( ! (status = module_usb_port_reset( module_usb_port_count )) ) continue;	// device doesn't exist anymore

					// remember max packet size
					module_usb_port[ module_usb_port_count ].max_packet_size = device_descriptor -> max_packet_size;

					// acquired default descriptor length
					module_usb_port[ module_usb_port_count ].default_descriptor_length = device_descriptor -> length;

					// set device address
					module_usb_descriptor( module_usb_port_count, 1, EMPTY, EMPTY );

					// assigned device address
					module_usb_port[ module_usb_port_count ].address_id = (uint64_t) ((uintptr_t) &module_usb_port[ module_usb_port_count ] - (uintptr_t) module_usb_port) / sizeof( struct MODULE_USB_STRUCTURE_PORT );

					// retrieve full device descriptor
					kernel -> memory_clean( (uint64_t *) device_descriptor, 1 );
					module_usb_descriptor( module_usb_port_count, 2, module_usb_port[ module_usb_port_count ].default_descriptor_length, (uintptr_t) device_descriptor & ~KERNEL_PAGE_mirror );
				}
			}
		}
	}

	// hold the door
	while( TRUE ) kernel -> time_sleep( TRUE );
}