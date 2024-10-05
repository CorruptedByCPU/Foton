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

uint16_t module_usb_hid_keyboard_matrix( uint8_t id ) {
	// translate key id, default: low matrix
	uint16_t key = module_usb_keyboard_matrix_low[ id ];

	// high matrix?
	if( module_usb_keyboard_cache[ 0 ] & (MODULE_USB_HID_KEYBOARD_KEY_CODE_SHIFT_LEFT | MODULE_USB_HID_KEYBOARD_KEY_CODE_SHIFT_RIGHT) )
		// yes
		key = module_usb_keyboard_matrix_high[ id ];

	// return key from matrix
	return key;
}

void module_usb_hid_keyboard_save( uint16_t key ) {
	// in first free space in keyboard buffer
	for( uint8_t c = 0; c < 8; c++ )
		// save character
		if( ! kernel -> device_keyboard[ c ] ) { kernel -> device_keyboard[ c ] = key; break; }
}

void module_usb_hid_keyboard( void ) {
	// prepare keyboard input cache
	uint8_t *cache = (uint8_t *) kernel -> memory_alloc_low( TRUE );

	// receive keys
	while( TRUE ) {
		// from any available keyboard
		for( uint8_t p = 0; p < MODULE_USB_PORT_limit; p++ ) {
			// registered controller with definied protocol?
			if( module_usb_port[ p ].type != MODULE_USB_DEVICE_TYPE_HID_KEYBOARD ) continue;	// no

			// clean up cache
			kernel -> memory_clean( (uint64_t *) cache, TRUE );

			// create input Transfer Descriptor
			uint8_t status = module_usb_uhci_descriptor_io( (struct MODULE_USB_STRUCTURE_PORT *) &module_usb_port[ p ], 0x08, (uintptr_t) cache & ~KERNEL_PAGE_mirror, MODULE_USB_UHCI_TD_PACKET_IDENTIFICATION_in, 16 );

			// if something bad hapenned
			if( status ) continue;	// ignore keys

			// check special key
			if( *cache != module_usb_keyboard_cache[ 0 ] ) {
				// key update
				uint16_t key;

				// CTRL left
				if( module_usb_keyboard_cache[ 0 ] != (*cache & MODULE_USB_HID_KEYBOARD_KEY_CODE_CTRL_LEFT) ) {
					// default state
					key = STD_KEY_CTRL_LEFT;

					// if released
					if( ! (*cache & MODULE_USB_HID_KEYBOARD_KEY_CODE_CTRL_LEFT) ) key |= STD_KEY_RELEASE;

					// insert key into cache
					module_usb_hid_keyboard_save( key );
				}

				// SHIFT left
				if( module_usb_keyboard_cache[ 0 ] != (*cache & MODULE_USB_HID_KEYBOARD_KEY_CODE_SHIFT_LEFT) ) {
					// default state
					key = STD_KEY_SHIFT_LEFT;

					// if released
					if( ! (*cache & MODULE_USB_HID_KEYBOARD_KEY_CODE_SHIFT_LEFT) ) key |= STD_KEY_RELEASE;
					
					// insert key into cache
					module_usb_hid_keyboard_save( key );
				}

				// ALT left
				if( module_usb_keyboard_cache[ 0 ] != (*cache & MODULE_USB_HID_KEYBOARD_KEY_CODE_ALT_LEFT) ) {
					// default state
					key = STD_KEY_ALT_LEFT;

					// if released
					if( ! (*cache & MODULE_USB_HID_KEYBOARD_KEY_CODE_ALT_LEFT) ) key |= STD_KEY_RELEASE;
					
					// insert key into cache
					module_usb_hid_keyboard_save( key );
				}

				// MENU left
				if( module_usb_keyboard_cache[ 0 ] != (*cache & MODULE_USB_HID_KEYBOARD_KEY_CODE_MENU_LEFT) ) {
					// default state
					key = STD_KEY_MENU;

					// if released
					if( ! (*cache & MODULE_USB_HID_KEYBOARD_KEY_CODE_MENU_LEFT) ) key |= STD_KEY_RELEASE;
					
					// insert key into cache
					module_usb_hid_keyboard_save( key );
				}

				// preserve special key state
				module_usb_keyboard_cache[ 0 ] = *cache;
			}

			// release key
			for( uint8_t i = 2; i < 8; i++ ) {
				// ignore empty fields
				if( ! module_usb_keyboard_cache[ i ] ) continue;

				// released by default
				uint8_t released = TRUE;

				// search for released key
				for( uint8_t j = 2; j < 8; j++ ) if( module_usb_keyboard_cache[ i ] == cache[ j ] ) released = FALSE;

				// released a key?
				if( ! released ) continue;	// no

				// insert key into kernel cache
				module_usb_hid_keyboard_save( module_usb_hid_keyboard_matrix( module_usb_keyboard_cache[ i ] ) | STD_KEY_RELEASE );
			}


			// press key
			for( uint8_t i = 2; i < 8; i++ ) {
				// ignore empty fields
				if( ! cache[ i ] ) continue;

				// not pressed by default
				uint8_t pressed = TRUE;

				// search for pressed key
				for( uint8_t j = 2; j < 8; j++ ) if( cache[ i ] == module_usb_keyboard_cache[ j ] ) pressed = FALSE;

				// pressed a key?
				if( ! pressed ) continue;	// no

				// insert key into kernel cache
				module_usb_hid_keyboard_save( module_usb_hid_keyboard_matrix( cache[ i ] ) );
			}

			// update local key cache
			for( uint8_t i = 2; i < 8; i++ ) module_usb_keyboard_cache[ i ] = cache[ i ];
		}
	}
}

void module_usb_hid_mouse( void ) {
	// prepare mouse input cache
	int8_t *cache = (int8_t *) kernel -> memory_alloc_low( TRUE );

	// receive properties
	while( TRUE ) {
		// from any available mouse
		for( uint8_t p = 0; p < MODULE_USB_PORT_limit; p++ ) {
			// registered controller with definied protocol?
			if( module_usb_port[ p ].type != MODULE_USB_DEVICE_TYPE_HID_MOUSE ) continue;	// no

			// clean up cache
			kernel -> memory_clean( (uint64_t *) cache, TRUE );

			// create input Transfer Descriptor
			module_usb_uhci_descriptor_io( (struct MODULE_USB_STRUCTURE_PORT *) &module_usb_port[ p ], 0x04, (uintptr_t) cache & ~KERNEL_PAGE_mirror, MODULE_USB_UHCI_TD_PACKET_IDENTIFICATION_in, 8 );

			// Buttons status
			kernel -> device_mouse_status = cache[ 0 ];

			// X axis

			// overflow from left?
			if( kernel -> device_mouse_x + cache[ 1 ] < 0 ) kernel -> device_mouse_x = EMPTY;

			// overflow from right?
			else if( kernel -> device_mouse_x + cache[ 1 ] > kernel -> framebuffer_width_pixel - 1 ) kernel -> device_mouse_x = kernel -> framebuffer_width_pixel - 1;
				
			// compound new position
			else kernel -> device_mouse_x += cache[ 1 ];

			// Y axis

			// overflow from top?
			if( kernel -> device_mouse_y + cache[ 2 ] < 0 ) kernel -> device_mouse_y = EMPTY;

			// overflow from bottom?
			else if( kernel -> device_mouse_y + cache[ 2 ] > kernel -> framebuffer_height_pixel - 1 ) kernel -> device_mouse_y = kernel -> framebuffer_height_pixel - 1;

			// compound new position
			else kernel -> device_mouse_y += cache[ 2 ];
		}
	}
}

struct MODULE_USB_STRUCTURE_PORT *module_usb_port_register( uint8_t c, uint8_t p ) {
	// locate already existing port configuration
	uint8_t limit = TRUE; for( ; limit < MODULE_USB_PORT_limit; limit++ ) if( module_usb_port[ limit ].id_controller == c && module_usb_port[ limit ].id_port == p ) return (struct MODULE_USB_STRUCTURE_PORT *) &module_usb_port[ limit ];

	// return first available port configuration
	for( uint8_t s = TRUE; s < MODULE_USB_PORT_limit; s++ )
		if( ! module_usb_port[ s ].status )
			// return port configuration pointer
			return (struct MODULE_USB_STRUCTURE_PORT *) &module_usb_port[ s ];

	// no available port configuration
	return EMPTY;
}

void module_usb_uhci_init( uint8_t c ) {
	// reset controller
	driver_port_out_word( module_usb_controller[ c ].base_address + offsetof( struct MODULE_USB_STRUCTURE_UHCI_REGISTER, command ), 0x0004 ); kernel -> time_sleep( 16 ); driver_port_out_word( module_usb_controller[ c ].base_address + offsetof( struct MODULE_USB_STRUCTURE_UHCI_REGISTER, command ), 0x0000 );

	// disable interrupts
	driver_port_out_word( module_usb_controller[ c ].base_address + offsetof( struct MODULE_USB_STRUCTURE_UHCI_REGISTER, interrupt_enable ), EMPTY );

	// reset frame number
	driver_port_out_word( module_usb_controller[ c ].base_address + offsetof( struct MODULE_USB_STRUCTURE_UHCI_REGISTER, frame_number ), EMPTY );

	// alloc area for frame list
	module_usb_controller[ c ].frame_base_address = (uint32_t) (kernel -> memory_alloc_low( TRUE ) & ~KERNEL_PAGE_mirror);
	driver_port_out_dword( module_usb_controller[ c ].base_address + offsetof( struct MODULE_USB_STRUCTURE_UHCI_REGISTER, frame_list_base_address ), module_usb_controller[ c ].frame_base_address );

	// fill up frame list with queues
	module_usb_uhci_queue( (uint32_t *) (module_usb_controller[ c ].frame_base_address | KERNEL_PAGE_mirror) );

	// clear status register
	driver_port_out_word( module_usb_controller[ c ].base_address + offsetof( struct MODULE_USB_STRUCTURE_UHCI_REGISTER, status ), 0b00111111 );

	// enable controller
	driver_port_out_word( module_usb_controller[ c ].base_address + offsetof( struct MODULE_USB_STRUCTURE_UHCI_REGISTER, command ), TRUE );
}

void module_usb_uhci_descriptor( struct MODULE_USB_STRUCTURE_PORT *port, uint8_t length, uintptr_t target, uint8_t flow, uintptr_t packet ) {
	// prepare Transfer Descriptors area
	struct MODULE_USB_STRUCTURE_UHCI_TD *td_pointer = (struct MODULE_USB_STRUCTURE_UHCI_TD *) kernel -> memory_alloc_low( TRUE );
	struct MODULE_USB_STRUCTURE_UHCI_TD *td = (struct MODULE_USB_STRUCTURE_UHCI_TD *) td_pointer;	// start from first TD

	// data Transfer Descriptor
	td -> flags = MODULE_USB_UHCI_QTD_FLAG_depth_first;
	// select descriptor which tells USB device where incomming data should be placed
	td -> link_pointer = (uintptr_t) (td + TRUE) >> 4;

	// descriptor active
	td -> status = MODULE_USB_UHCI_TD_STATUS_active;

	// set device speed
	td -> low_speed = port -> low_speed;
	// default error counter
	td -> error_counter = STD_MAX_unsigned;

	// set Packet Identification as
	td -> packet_identification = MODULE_USB_UHCI_TD_PACKET_IDENTIFICATION_setup;
	// set Device Identification
	td -> device_address = port -> id_address;
	// set Endpoint Identification
	td -> endpoint = port -> id_endpoint;
	// packet length
	td -> max_length = 0x07;

	// location of packet properties
	td -> buffer_pointer = packet;

	//----------------------------------------------------------------------

	// TD semaphore
	port -> toggle = TRUE;

	// change Transfer Descriptor Itendification by existing length and request
	uint8_t length_semaphore = FALSE;
	if( length && packet ) length_semaphore = TRUE;

	// create Transfer Descriptors until end of length
	while( length ) {
		// next Transfer Descriptor
		td++;

		// data Transfer Descriptor
		td -> flags = MODULE_USB_UHCI_QTD_FLAG_depth_first;
		// select descriptor which tells USB device where incomming data should be placed
		td -> link_pointer = (uintptr_t) (td + TRUE) >> 4;

		// descriptor active
		td -> status = MODULE_USB_UHCI_TD_STATUS_active;
		// set device speed
		td -> low_speed = port -> low_speed;
		// default error counter
		td -> error_counter = STD_MAX_unsigned;

		// set Packet Identification as
		td -> packet_identification = flow;
		// set Device Identification
		td -> device_address = port -> id_address;
		// set Endpoint Identification
		td -> endpoint = port -> id_endpoint;
		// TD semaphore
		td -> data_toggle = port -> toggle;
		// receive a chunk of requested Bytes
		if( length > port -> max_packet_length ) {
			// requested data length to be received
			td -> max_length = port -> max_packet_length - 1;

			// how much Bytes to receive in next Transfer Descriptor
			length -= port -> max_packet_length;
		} else {
			// requested data length to be received
			td -> max_length = length - 1;

			// nothing more to do
			length = EMPTY;
		}

		// location of chunk of retrieved data
		td -> buffer_pointer = target;

		// next chunk
		target += port -> max_packet_length;

		// next TD semaphore state
		if( port -> toggle ) port -> toggle = FALSE; else port -> toggle = TRUE;
	}

	//----------------------------------------------------------------------

	// next Transfer Descriptor
	td++;

	// terminate Transfer Descriptor
	td -> flags = MODULE_USB_UHCI_QTD_FLAG_terminate;
	// nothing more to do
	td -> link_pointer = EMPTY;

	// descriptor active
	td -> status = MODULE_USB_UHCI_TD_STATUS_active;
	// set device speed
	td -> low_speed = port -> low_speed;
	// default error counter
	td -> error_counter = STD_MAX_unsigned;

	// set Packet Identification as
	if( length_semaphore ) td -> packet_identification = MODULE_USB_UHCI_TD_PACKET_IDENTIFICATION_out; else td -> packet_identification = MODULE_USB_UHCI_TD_PACKET_IDENTIFICATION_in;
	// set Device Identification
	td -> device_address = port -> id_address;
	// set Endpoint Identification
	td -> endpoint = port -> id_endpoint;
	// TD semaphore
	td -> data_toggle = TRUE;	// last descriptor is always TRUE
	// no more data to be transferred
	td -> max_length = STD_MAX_unsigned;

	// nothing to transfer
	td -> buffer_pointer = EMPTY;

	//----------------------------------------------------------------------

	// insert Transfer Descriptors on Queue
	uint64_t entry = module_usb_uhci_queue_insert( 1, EMPTY, (uintptr_t) td_pointer );

	// wait for device
	while( td -> status & 0b10000000 );

	// remove Transfer Descriptors from Queue
	module_usb_uhci_queue_remove( 1, entry );

	// relase Transfer Descriptors list
	kernel -> memory_release( (uintptr_t) td_pointer, TRUE );
}

uint8_t module_usb_uhci_descriptor_io( struct MODULE_USB_STRUCTURE_PORT *port, uint8_t length, uintptr_t target, uint8_t flow, uint8_t queue ) {
	// prepare Transfer Descriptors area
	struct MODULE_USB_STRUCTURE_UHCI_TD *td = (struct MODULE_USB_STRUCTURE_UHCI_TD *) kernel -> memory_alloc_low( TRUE );

	//----------------------------------------------------------------------

	// data Transfer Descriptor
	td -> flags = MODULE_USB_UHCI_QTD_FLAG_terminate;
	// select descriptor which tells USB device where incomming data should be placed
	td -> link_pointer = (uintptr_t) (td + TRUE) >> 4;

	// descriptor active
	td -> status = MODULE_USB_UHCI_TD_STATUS_active;
	// set device speed
	td -> low_speed = port -> low_speed;
	// default error counter
	td -> error_counter = STD_MAX_unsigned;

	// set Packet Identification as
	td -> packet_identification = flow;
	// set Device Identification
	td -> device_address = port -> id_address;
	// set Endpoint Identification
	td -> endpoint = port -> id_endpoint + 1;
	// TD semaphore
	td -> data_toggle = port -> toggle;
	// requested data length to be received
	td -> max_length = length - 1;

	// location of chunk of retrieved data
	td -> buffer_pointer = target;

	// next TD semaphore state
	if( port -> toggle ) port -> toggle = FALSE; else port -> toggle = TRUE;

	//----------------------------------------------------------------------

	// insert Transfer Descriptors on Queue
	uint64_t entry = module_usb_uhci_queue_insert( queue, EMPTY, (uintptr_t) td );

	// wait for device
	while( td -> status & 0b10000000 ) kernel -> time_sleep( 1 );

	// remember status of Transfer Descriptor
	volatile uint8_t status = td -> status;

	// release Transfer Descriptor area
	kernel -> memory_release( (uintptr_t) td, TRUE );

	// return Transfer Descriptor status
	return status;
}

uint16_t module_usb_uhci_device_init( uint8_t c, uint8_t p ) {
	// send command, RESET
	driver_port_out_word( module_usb_controller[ c ].base_address + offsetof( struct MODULE_USB_STRUCTURE_UHCI_REGISTER, port[ p ] ), MODULE_USB_UHCI_PORT_STATUS_AND_CONTROL_port_reset ); kernel -> time_sleep( 64 );
	driver_port_out_word( module_usb_controller[ c ].base_address + offsetof( struct MODULE_USB_STRUCTURE_UHCI_REGISTER, port[ p ] ), EMPTY ); kernel -> time_sleep( 16 );

	// try to enable attached device
	for( uint8_t i = 0; i < 7; i++ ) {
		// retrieve port status
		uint16_t status = driver_port_in_word( module_usb_controller[ c ].base_address + offsetof( struct MODULE_USB_STRUCTURE_UHCI_REGISTER, port[ p ] ) );

		// debug
		// kernel -> log( (uint8_t *) "[USB].%u Port%u - Status:%16b\n", c, p, status );

		// device attached?
		if( ! (status & MODULE_USB_UHCI_PORT_STATUS_AND_CONTROL_current_connect_status) ) {	// no
			// ignore port
			return EMPTY;
		}
	
		// port status change?
		if( status & (MODULE_USB_UHCI_PORT_STATUS_AND_CONTROL_connect_status_change | MODULE_USB_UHCI_PORT_STATUS_AND_CONTROL_port_enable_change) ) {
			// clean it
			driver_port_out_word( module_usb_controller[ c ].base_address + offsetof( struct MODULE_USB_STRUCTURE_UHCI_REGISTER, port[ p ] ), MODULE_USB_UHCI_PORT_STATUS_AND_CONTROL_connect_status_change | MODULE_USB_UHCI_PORT_STATUS_AND_CONTROL_port_enable_change ); kernel -> time_sleep( 1 );

			// try again
			continue;
		}
		
		// port enabled?
		if( status & MODULE_USB_UHCI_PORT_STATUS_AND_CONTROL_port_enabled )
			// done
			return status;
		
		// try to enable port
		driver_port_out_word( module_usb_controller[ c ].base_address + offsetof( struct MODULE_USB_STRUCTURE_UHCI_REGISTER, port[ p ] ), MODULE_USB_UHCI_PORT_STATUS_AND_CONTROL_port_enabled );
	}

	// no device attached
	return EMPTY;
}

uint8_t module_usb_uhci_device_setup( struct MODULE_USB_STRUCTURE_PORT *port ) {
	// allocate area for requested data
	uintptr_t descriptor_default = kernel -> memory_alloc_low( TRUE );

	// DEFAULT DESCRIPTOR --------------------------------------------------

	// prepare packet
	struct MODULE_USB_STRUCTURE_UHCI_PACKET *packet = (struct MODULE_USB_STRUCTURE_UHCI_PACKET *) kernel -> memory_alloc_low( TRUE );
	packet -> type		= MODULE_USB_PACKET_TYPE_direction_device_to_host;
	packet -> request	= MODULE_USB_PACKET_REQUEST_descriptor_get;
	packet -> value		= MODULE_USB_PACKET_VALUE_descriptor_type_device;
	packet -> length	= 0x08;	// default first request length

	// retrieve default descriptor from device
	module_usb_uhci_descriptor( port, 0x08, descriptor_default & ~KERNEL_PAGE_mirror, MODULE_USB_UHCI_TD_PACKET_IDENTIFICATION_in, (uintptr_t) packet );

	// properties of device descriptor
	struct MODULE_USB_STRUCTURE_UHCI_DESCRIPTOR_DEVICE *descriptor_device = (struct MODULE_USB_STRUCTURE_UHCI_DESCRIPTOR_DEVICE *) descriptor_default;

	// device port reset
	if( ! module_usb_uhci_device_init( port -> id_controller, port -> id_port ) ) return FALSE;	// device doesn't exist anymore

	// ENUMERATE -----------------------------------------------------------

	// prepare packet
	packet -> type		= MODULE_USB_PACKET_TYPE_direction_host_to_device;
	packet -> request	= MODULE_USB_PACKET_REQUEST_address_set;
	packet -> value		= ((uintptr_t) port - (uintptr_t) module_usb_port) / sizeof( struct MODULE_USB_STRUCTURE_PORT );
	packet -> length	= EMPTY;

	// set device address
	module_usb_uhci_descriptor( port, EMPTY, EMPTY, EMPTY, (uintptr_t) packet );

	// remember device address
	port -> id_address = ((uintptr_t) port - (uintptr_t) module_usb_port) / sizeof( struct MODULE_USB_STRUCTURE_PORT );

	// DEVICE DESCRIPTOR ---------------------------------------------------

	// prepare packet
	packet -> type		= MODULE_USB_PACKET_TYPE_direction_device_to_host;
	packet -> request	= MODULE_USB_PACKET_REQUEST_descriptor_get;
	packet -> value		= MODULE_USB_PACKET_VALUE_descriptor_type_device;
	packet -> length	= descriptor_device -> length;

	// retrieve full device descriptor
	module_usb_uhci_descriptor( port, descriptor_device -> length, descriptor_default & ~KERNEL_PAGE_mirror, MODULE_USB_UHCI_TD_PACKET_IDENTIFICATION_in, (uintptr_t) packet );

	// CONFIGURATION/INTERFACE/ENDPOINT DESCRIPTORS ------------------------

	// prepare packet
	packet -> type		= MODULE_USB_PACKET_TYPE_direction_device_to_host;
	packet -> request	= MODULE_USB_PACKET_REQUEST_descriptor_get;
	packet -> value		= MODULE_USB_PACKET_VALUE_descriptor_type_configuration;
	packet -> length	= sizeof( struct MODULE_USB_STRUCTURE_UHCI_DESCRIPTOR_CONFIGURATION );

	// retrieve part of configuration properties
	module_usb_uhci_descriptor( port, sizeof( struct MODULE_USB_STRUCTURE_UHCI_DESCRIPTOR_CONFIGURATION ), descriptor_default & ~KERNEL_PAGE_mirror, MODULE_USB_UHCI_TD_PACKET_IDENTIFICATION_in, (uintptr_t) packet );

	// properties of configuration descriptor
	struct MODULE_USB_STRUCTURE_UHCI_DESCRIPTOR_CONFIGURATION *descriptor_configuration = (struct MODULE_USB_STRUCTURE_UHCI_DESCRIPTOR_CONFIGURATION *) descriptor_default;

	// prepare packet
	packet -> length = descriptor_configuration -> total_length;

	// retrieve full configuration properties
	module_usb_uhci_descriptor( port, descriptor_configuration -> total_length, descriptor_default & ~KERNEL_PAGE_mirror, MODULE_USB_UHCI_TD_PACKET_IDENTIFICATION_in, (uintptr_t) packet );

	// prepare packet
	packet -> type		= MODULE_USB_PACKET_TYPE_direction_host_to_device;
	packet -> request	= MODULE_USB_PACKET_REQUEST_configuration_set;
	packet -> value		= descriptor_configuration -> config_value;
	packet -> length	= EMPTY;

	// select first configuration as default
	module_usb_uhci_descriptor( port, EMPTY, EMPTY, EMPTY, (uintptr_t) packet );

	// properties of interface descriptor
	struct MODULE_USB_STRUCTURE_UHCI_DESCRIPTOR_INTERFACE *descriptor_interface = EMPTY;

	// properties of endpoint descriptor
	struct MODULE_USB_STRUCTURE_UHCI_DESCRIPTOR_ENDPOINT *descriptor_endpoint = EMPTY;

	// parse configuration/interface/endpoint descriptors
	struct MODULE_USB_STRUCTURE_UHCI_DESCRIPTOR_DEFAULT *parse = (struct MODULE_USB_STRUCTURE_UHCI_DESCRIPTOR_DEFAULT *) descriptor_default;
	while( parse -> length ) {
		// configuration descriptor
		if( parse -> type == 0x02 ) {
			// properties of configuration descriptor
			descriptor_configuration = (struct MODULE_USB_STRUCTURE_UHCI_DESCRIPTOR_CONFIGURATION *) parse;

			// debug
			kernel -> log( (uint8_t *) "[USB].%u Port%u - Configuration ID:%u, %u Interface/s", port -> id_controller, port -> id_port, descriptor_configuration -> config_value, descriptor_configuration -> interface_count ); if( (descriptor_configuration -> attributes >> 5) & TRUE ) kernel -> log( (uint8_t *) ", Remote Wakeup" ); if( (descriptor_configuration -> attributes >> 6) & TRUE ) kernel -> log( (uint8_t *) ", Self Powered" ); kernel -> log( (uint8_t *) ", Maximum Power Consumption %umA\n", descriptor_configuration -> max_power << STD_SHIFT_2 );
		} else

		// interface descriptor
		if( parse -> type == 0x04 ) {
			// properties of interface descriptor
			descriptor_interface = (struct MODULE_USB_STRUCTURE_UHCI_DESCRIPTOR_INTERFACE *) parse;

			// debug
			kernel -> log( (uint8_t *) "[USB].%u Port%u - Interface ID:%u, %u Endpoint/s", port -> id_controller, port -> id_port, descriptor_interface -> interface_id, descriptor_interface -> endpoint_count ); kernel -> log( (uint8_t *) ", Class: " ); switch( descriptor_interface -> class ) { case 0x03: { kernel -> log( (uint8_t *) "HID" ); if( descriptor_interface -> protocol == 0x01 ) kernel -> log( (uint8_t *) " (Protocol: Keyboard)" ); if( descriptor_interface -> protocol == 0x02 ) kernel -> log( (uint8_t *) " (Protocol: Mouse)" ); if( descriptor_interface -> subclass == 1 ) kernel -> log( (uint8_t *) ", Subclass: Boot Interface" ); else kernel -> log( (uint8_t *) ", Subclass: {unknown}" ); break; } case 0x07: { kernel -> log( (uint8_t *) "Printer" ); break; } case 0x08: { kernel -> log( (uint8_t *) "Mass Storage" ); if( descriptor_interface -> subclass == 0x06 ) kernel -> log( (uint8_t *) ", Subclass: SCSI transparent command set" ); else kernel -> log( (uint8_t *) ", Subclass: {unknown}" ); break; } case 0x09: { kernel -> log( (uint8_t *) "HUB" ); break; } default: kernel -> log( (uint8_t *) "{unknown}" ); } kernel -> log( (uint8_t *) "\n" );
		} else

		// endpoint descriptor
		if( parse -> type == 0x05 ) {
			// properties of endpoint descriptor
			descriptor_endpoint = (struct MODULE_USB_STRUCTURE_UHCI_DESCRIPTOR_ENDPOINT *) parse;

			// debug
			kernel -> log( (uint8_t *) "[USB].%u Port%u - Endpoint ID:%u", port -> id_controller, port -> id_port, descriptor_endpoint -> address & 0x1111 ); if( descriptor_endpoint -> address >> 7 ) kernel -> log( (uint8_t *) ", In" ); else kernel -> log( (uint8_t *) ", Out" ); switch( descriptor_endpoint -> attributes & 0x11 ) { case 0x01: { kernel -> log( (uint8_t *) ", Isochronous" ); break; } case 0x02: { kernel -> log( (uint8_t *) ", Bulk" ); break; } case 0x03: { kernel -> log( (uint8_t *) ", Interrupt" ); break; } default: kernel -> log( (uint8_t *) ", Control" ); } kernel -> log( (uint8_t *) ", Max Packet Size %u Byte/s", descriptor_endpoint -> max_packet_size ); kernel -> log( (uint8_t *) "\n" );
		}

		// next
		parse = (struct MODULE_USB_STRUCTURE_UHCI_DESCRIPTOR_DEFAULT *) ((uintptr_t) parse + parse -> length);

		// found first descriptors of each type?
		if( descriptor_configuration && descriptor_interface && descriptor_endpoint ) break;	// yes
	}

	// set device type
	if( descriptor_interface -> class == 0x03 && descriptor_interface -> protocol == 0x01 )	port -> type = MODULE_USB_DEVICE_TYPE_HID_KEYBOARD;
	if( descriptor_interface -> class == 0x03 && descriptor_interface -> protocol == 0x02 )	port -> type = MODULE_USB_DEVICE_TYPE_HID_MOUSE;
	if( descriptor_interface -> class == 0x08 )						port -> type = MODULE_USB_DEVICE_TYPE_HID_STORAGE;

	// prepare packet
	packet -> type		= MODULE_USB_PACKET_TYPE_direction_host_to_device | MODULE_USB_PACKET_TYPE_subtype_standard | MODULE_USB_PACKET_TYPE_recipient_interface;
	packet -> request	= MODULE_USB_PACKET_REQUEST_interface_set;
	packet -> value		= descriptor_interface -> interface_id;
	packet -> length	= EMPTY;

	// select first interface as default
	module_usb_uhci_descriptor( port, EMPTY, EMPTY, EMPTY, (uintptr_t) packet );

	// prepare packet
	packet -> type		= MODULE_USB_PACKET_TYPE_direction_host_to_device | MODULE_USB_PACKET_TYPE_subtype_class | MODULE_USB_PACKET_TYPE_recipient_interface;
	packet -> request	= MODULE_USB_PACKET_REQUEST_idle_set;
	packet -> value		= EMPTY;	// Indefinitiely and All Reports
	packet -> index		= descriptor_interface -> interface_id;
	packet -> length	= EMPTY;

	// set interface as IDLE
	module_usb_uhci_descriptor( port, EMPTY, EMPTY, EMPTY, (uintptr_t) packet );

	// TD semaphore, reset
	port -> toggle = FALSE;

	// debug
	// if( port -> type == MODULE_USB_DEVICE_TYPE_HID_KEYBOARD ) module_usb_hid_keyboard();

	// done
	return TRUE;
}

void module_usb_uhci_queue( uint32_t *frame_list ) {
	// acquire 1/1024u query
	module_usb_uhci_queue_1u = (struct MODULE_USB_STRUCTURE_UHCI_QUEUE *) module_usb_uhci_queue_empty();

	// acquire 8/1024u query
	module_usb_uhci_queue_8u = (struct MODULE_USB_STRUCTURE_UHCI_QUEUE *) module_usb_uhci_queue_empty();

	// acquire 16/1024u query
	module_usb_uhci_queue_16u = (struct MODULE_USB_STRUCTURE_UHCI_QUEUE *) module_usb_uhci_queue_empty();

	// connect 8u > 1u
	module_usb_uhci_queue_insert( 8, MODULE_USB_UHCI_QTD_FLAG_queue, (uintptr_t) module_usb_uhci_queue_1u );

	// connect 16u > 8u
	module_usb_uhci_queue_insert( 16, MODULE_USB_UHCI_QTD_FLAG_queue, (uintptr_t) module_usb_uhci_queue_8u );

	// insert queues
	for( uint64_t i = 0; i < STD_PAGE_byte >> STD_SHIFT_4; i++ ) {
		// 1u
		frame_list[ i ] = (uintptr_t) module_usb_uhci_queue_1u | MODULE_USB_UHCI_QTD_FLAG_queue;

		// 8u
		if( ! ((i + 1) % 8) ) frame_list[ i ] = (uintptr_t) module_usb_uhci_queue_8u | MODULE_USB_UHCI_QTD_FLAG_queue;

		// 16u
		if( ! ((i + 1) % 16) ) frame_list[ i ] = (uintptr_t) module_usb_uhci_queue_16u | MODULE_USB_UHCI_QTD_FLAG_queue;
	}
}

uintptr_t module_usb_uhci_queue_empty( void ) {
	// assign queue area
	struct MODULE_USB_STRUCTURE_UHCI_QUEUE *queue = (struct MODULE_USB_STRUCTURE_UHCI_QUEUE *) kernel -> memory_alloc_low( TRUE );

	// fill with empty entries
	for( uint64_t i = 0; i < STD_PAGE_byte / sizeof( struct MODULE_USB_STRUCTURE_UHCI_QUEUE ); i++ ) {
		// next entry
		queue[ i ].head_link_pointer_and_flags = MODULE_USB_UHCI_QTD_FLAG_terminate;

		// and current
		queue[ i ].element_link_pointer_and_flags = MODULE_USB_UHCI_QTD_FLAG_terminate;

		// preserved head and element
		queue[ i ].preserved_head = MODULE_USB_UHCI_QTD_FLAG_terminate;
		queue[ i ].preserved_element = MODULE_USB_UHCI_QTD_FLAG_terminate;
	}

	// return address of queue
	return (uintptr_t) queue;
}

uint64_t module_usb_uhci_queue_insert( uint8_t unit, uint8_t type, uintptr_t source ) {
	// properties of default queue
	struct MODULE_USB_STRUCTURE_UHCI_QUEUE *queue = EMPTY;

	// select target queue
	switch( unit ) {
		// queue 1u
		case 1: { queue = module_usb_uhci_queue_1u; break; }
		// queue 8u
		case 8: { queue = module_usb_uhci_queue_8u; break; }
		// queue 16u
		case 16: { queue = module_usb_uhci_queue_16u; break; }
	}

	// search every entry
	while( TRUE ) for( uint64_t i = 0; i < STD_PAGE_byte / sizeof( struct MODULE_USB_STRUCTURE_UHCI_QUEUE ); i++ ) {
		// available?
		if( queue[ i ].element_link_pointer_and_flags & MODULE_USB_UHCI_QTD_FLAG_terminate ) {
			if( type == MODULE_USB_UHCI_QTD_FLAG_queue ) {
				// insert Queue
				queue[ i ].head_link_pointer_and_flags = source | type;

				// remember Queue properties
				queue[ i ].preserved_head = source | type;
			} else
				// insert Transfer Descriptors
				queue[ i ].element_link_pointer_and_flags = source | type;

			// next entry already occupied?
			if( ! (queue[ i + 1 ].element_link_pointer_and_flags & MODULE_USB_UHCI_QTD_FLAG_terminate)  )
				// make a path to next one
				queue[ i ].head_link_pointer_and_flags = (uintptr_t) &queue[ i + 1 ] | MODULE_USB_UHCI_QTD_FLAG_queue;

			// return entry id
			return i;
		}
	}
}

void module_usb_uhci_queue_remove( uint8_t unit, uint64_t entry ) {
	// properties of default queue
	struct MODULE_USB_STRUCTURE_UHCI_QUEUE *queue = EMPTY;

	// select target queue
	switch( unit ) {
		// queue 1u
		case 1: { queue = module_usb_uhci_queue_1u; break; }
		// queue 8u
		case 8: { queue = module_usb_uhci_queue_8u; break; }
		// queue 16u
		case 16: { queue = module_usb_uhci_queue_16u; break; }
	}

	if( queue[ entry + 1 ].head_link_pointer_and_flags & MODULE_USB_UHCI_QTD_FLAG_terminate )
		// truncate queue
		queue[ entry ].head_link_pointer_and_flags = MODULE_USB_UHCI_QTD_FLAG_terminate;
	else
		// create link to next entry
		queue[ entry + 1 ].element_link_pointer_and_flags = (uintptr_t) &queue[ entry + 1 ] | MODULE_USB_UHCI_QTD_FLAG_queue;
}

void _entry( uintptr_t kernel_ptr ) {
	// preserve kernel structure pointer
	kernel = (struct KERNEL *) kernel_ptr;

	// assign area for discovered USB controllers
	module_usb_controller = (struct MODULE_USB_STRUCTURE_CONTROLLER *) kernel -> memory_alloc_low( MACRO_PAGE_ALIGN_UP( sizeof( struct MODULE_USB_STRUCTURE_CONTROLLER ) * MODULE_USB_CONTROLLER_limit ) >> STD_SHIFT_PAGE );

	// assign area for discovered USB ports
	module_usb_port = (struct MODULE_USB_STRUCTURE_PORT *) kernel -> memory_alloc_low( MACRO_PAGE_ALIGN_UP( sizeof( struct MODULE_USB_STRUCTURE_PORT ) * MODULE_USB_PORT_limit ) >> STD_SHIFT_PAGE );

	// check every bus;device;function of PCI controller
	for( uint16_t b = 0; b < TRUE; b++ )
		for( uint8_t d = 0; d < 32; d++ )
			for( uint8_t f = 0; f < 8; f++ ) {
				// PCI properties
				struct DRIVER_PCI_STRUCTURE pci = { EMPTY, b, d, f };

				// if found
				if( (driver_pci_read( pci, DRIVER_PCI_REGISTER_class_and_subclass ) >> 8) == DRIVER_PCI_CLASS_SUBCLASS_usb_uhci ) {
					// debug
					kernel -> log( (uint8_t *) "[USB].%u PCI %2X:%2X.%u - USB controller found. (UHCI - Universal Host Controller Interface)\n", module_usb_controller_limit, pci.bus, pci.device, pci.function );

					// set type and base address
					module_usb_controller[ module_usb_controller_limit ].type = MODULE_USB_CONTROLLER_TYPE_UHCI;
					module_usb_controller[ module_usb_controller_limit ].base_address = driver_pci_read( pci, DRIVER_PCI_REGISTER_bar4 );

					// detect length of port area
					driver_pci_write( pci, DRIVER_PCI_REGISTER_bar4, 0xFFFFFFFF );
					module_usb_controller[ module_usb_controller_limit ].limit = ( ~( driver_pci_read( pci, DRIVER_PCI_REGISTER_bar4 ) & ~1 ) + 1 ) >> STD_SHIFT_16;
			
					// limit undefinied?
					if( ! (module_usb_controller[ module_usb_controller_limit ].base_address & TRUE) || ! module_usb_controller[ module_usb_controller_limit ].limit ) {
						// debug
						kernel -> log( (uint8_t *) "[USB].%u PCI %2X:%2X.%u - MMIO no support.\n", module_usb_controller_limit, pci.bus, pci.device, pci.function );

						// next controller
						continue;
					}

					// remove MMIO flag, if exist
					module_usb_controller[ module_usb_controller_limit ].base_address &= ~TRUE;

					// restore original value
					driver_pci_write( pci, DRIVER_PCI_REGISTER_bar4, module_usb_controller[ module_usb_controller_limit ].base_address );

					// disable BIOS legacy support
					driver_pci_write( pci, 0xC0, 0x8F00 );

					// controller registered
					module_usb_controller_limit++;
				}

				// if found
				if( (driver_pci_read( pci, DRIVER_PCI_REGISTER_class_and_subclass ) >> 8) == DRIVER_PCI_CLASS_SUBCLASS_usb_ohci ) {
					// debug
					kernel -> log( (uint8_t *) "[USB] PCI %2X:%2X.%u - USB controller found. (OHCI - Open Host Controller Interface)\n", pci.bus, pci.device, pci.function );
				}

				// if found
				if( (driver_pci_read( pci, DRIVER_PCI_REGISTER_class_and_subclass ) >> 8) == DRIVER_PCI_CLASS_SUBCLASS_usb_ehci ) {
					// debug
					kernel -> log( (uint8_t *) "[USB] PCI %2X:%2X.%u - USB controller found. (EHCI - Enchanced Host Controller Interface)\n", pci.bus, pci.device, pci.function );
				}

				// if found
				if( (driver_pci_read( pci, DRIVER_PCI_REGISTER_class_and_subclass ) >> 8) == DRIVER_PCI_CLASS_SUBCLASS_usb_xhci ) {
					// debug
					kernel -> log( (uint8_t *) "[USB] PCI %2X:%2X.%u - USB controller found. (xHCI - eXtensible Host Controller Interface)\n", pci.bus, pci.device, pci.function );
				}
			}

	// initialize UHCI controllers
	for( uint8_t c = 0; c < module_usb_controller_limit; c++ ) if( module_usb_controller[ c ].type == MODULE_USB_CONTROLLER_TYPE_UHCI ) module_usb_uhci_init( c );

	// initialize connected devices
	for( size_t c = 0; c < module_usb_controller_limit; c++ ) {
		for( size_t p = 0; p < module_usb_controller[ c ].limit; p++ ) {
			// initialize ports at UHCI controller
			if( module_usb_controller[ c ].type == MODULE_USB_CONTROLLER_TYPE_UHCI ) {
				// device status
				uint16_t status = EMPTY;

				// initialize attached device
				if( ! (status = module_usb_uhci_device_init( c, p )) ) continue;	// no device attached

				// register port on controller
				struct MODULE_USB_STRUCTURE_PORT *port = module_usb_port_register( c, p );

				// set port status
				port -> status = status;

				// remember port identification IDs
				port -> id_controller = c;
				port -> id_port = p;

				// retrieve port speed
				port -> low_speed = status >> 8;

				// default packet size
				port -> max_packet_length = 0x08;

				// configure device
				if( ! module_usb_uhci_device_setup( port ) ) {
					// couldn't setup device at port
					port -> status = EMPTY;

					// debug
					kernel -> log( (uint8_t *) "[USB].%u Port%u - No device.\n", c, p );

					// next device
					continue;
				}

				// debug
				kernel -> log( (uint8_t *) "[USB].%u Port%u - Attached, ", c, p );

				// debug
				if( port -> low_speed ) kernel -> log( (uint8_t *) "Low-Speed\n" );
				else kernel -> log( (uint8_t *) "Full-Speed\n" );
			}
		}
	}

	// enable mouse thread
	uint8_t module_usb_string_hid_mouse[] = "usb.ko - mouse";
	kernel -> module_thread( (uintptr_t) &module_usb_hid_mouse, (uint8_t *) &module_usb_string_hid_mouse, sizeof( module_usb_string_hid_mouse ) - 1 );

	// enable keyboard thread
	uint8_t module_usb_string_hid_keyboard[] = "usb.ko - keyboard";
	kernel -> module_thread( (uintptr_t) &module_usb_hid_keyboard, (uint8_t *) &module_usb_string_hid_keyboard, sizeof( module_usb_string_hid_keyboard ) - 1 );

	// hold the door
	while( TRUE ) kernel -> time_sleep( TRUE );
}