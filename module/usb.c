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
	module_usb_queue_1u = (struct MODULE_USB_STRUCTURE_QUEUE *) module_usb_queue_empty();

	// acquire 8/1024u query
	module_usb_queue_8u = (struct MODULE_USB_STRUCTURE_QUEUE *) module_usb_queue_empty();

	// acquire 16/1024u query
	module_usb_queue_16u = (struct MODULE_USB_STRUCTURE_QUEUE *) module_usb_queue_empty();

	// connect 8u > 1u
	// module_usb_queue_8u[ 0 ].element_link_pointer_and_flags = (uintptr_t) module_usb_queue_1u | MODULE_USB_DEFAULT_FLAG_queue;
	// module_usb_queue_8u[ 0 ].head_link_pointer_and_flags = (uintptr_t) module_usb_queue_16u | MODULE_USB_DEFAULT_FLAG_queue;

	// connect 16u > 8u
	// module_usb_queue_16u[ 0 ].element_link_pointer_and_flags = (uintptr_t) module_usb_queue_8u | MODULE_USB_DEFAULT_FLAG_queue;

	// for every 16th entry inside frame list
	for( uint64_t i = 15; i < STD_PAGE_byte >> STD_SHIFT_32; i += 16 )
		// insert 16u queue
		frame_list[ i ] = (uintptr_t) module_usb_queue_16u | MODULE_USB_DEFAULT_FLAG_queue;

	// for every 8th entry inside frame list
	for( uint64_t i = 7; i < STD_PAGE_byte >> STD_SHIFT_32; i += 8 )
		// insert 8u queue
		frame_list[ i ] = (uintptr_t) module_usb_queue_8u | MODULE_USB_DEFAULT_FLAG_queue;

	// for each entry inside frame list
	for( uint64_t i = 0; i < STD_PAGE_byte >> STD_SHIFT_32; i += 2 )
		// insert 1u queue
		frame_list[ i ] = (uintptr_t) module_usb_queue_1u | MODULE_USB_DEFAULT_FLAG_queue;
}

void module_usb_descriptor( uint8_t port, uint8_t length, uintptr_t target, uint8_t flow, uintptr_t packet ) {
	// prepare Transfer Descriptors area
	struct MODULE_USB_STRUCTURE_TD *td_pointer = (struct MODULE_USB_STRUCTURE_TD *) kernel -> memory_alloc_low( TRUE );
	struct MODULE_USB_STRUCTURE_TD *td = (struct MODULE_USB_STRUCTURE_TD *) td_pointer;	// start from first TD

	// data Transfer Descriptor
	td -> flags = MODULE_USB_DEFAULT_FLAG_data;
	// select descriptor which tells USB device where incomming data should be placed
	td -> link_pointer = (uintptr_t) (td + TRUE) >> 4;

	// descriptor active
	td -> status = MODULE_USB_TD_STATUS_active;
	// set device speed
	td -> low_speed = module_usb_port[ port ].low_speed;
	// default error counter
	td -> error_counter = STD_MAX_unsigned;

	// set Packet Identification as
	td -> packet_identification = MODULE_USB_TD_PACKET_IDENTIFICATION_setup;
	// set Device Identification
	td -> device_address = module_usb_port[ port ].address_id;
	// set Endpoint Identification
	td -> endpoint = module_usb_port[ port ].endpoint_id;
	// packet length
	td -> max_length = 0x07;

	// location of packet properties
	td -> buffer_pointer = packet;

	//----------------------------------------------------------------------

	// TD semaphore
	module_usb_port[ port ].toggle = TRUE;

	// change Transfer Descriptor Itendification by existing length and request
	uint8_t length_semaphore = FALSE;
	if( length && packet ) length_semaphore = TRUE;

	// create Transfer Descriptors until end of length
	while( length ) {
		// next Transfer Descriptor
		td++;

		// data Transfer Descriptor
		td -> flags = MODULE_USB_DEFAULT_FLAG_data;
		// select descriptor which tells USB device where incomming data should be placed
		td -> link_pointer = (uintptr_t) (td + TRUE) >> 4;

		// descriptor active
		td -> status = MODULE_USB_TD_STATUS_active;
		// set device speed
		td -> low_speed = module_usb_port[ port ].low_speed;
		// default error counter
		td -> error_counter = STD_MAX_unsigned;

		// set Packet Identification as
		td -> packet_identification = flow;
		// set Device Identification
		td -> device_address = module_usb_port[ port ].address_id;
		// set Endpoint Identification
		td -> endpoint = module_usb_port[ port ].endpoint_id;
		// TD semaphore
		td -> data_toggle = module_usb_port[ port ].toggle;
		// receive a chunk of requested Bytes
		if( length > module_usb_port[ port ].max_packet_size ) {
			// requested data length to be received
			td -> max_length = module_usb_port[ port ].max_packet_size - 1;

			// how much Bytes to receive in next Transfer Descriptor
			length -= module_usb_port[ port ].max_packet_size;
		} else {
			// requested data length to be received
			td -> max_length = length - 1;

			// nothing more to do
			length = EMPTY;
		}

		// location of chunk of retrieved data
		td -> buffer_pointer = target;

		// next chunk
		target += module_usb_port[ port ].max_packet_size;

		// next TD semaphore state
		if( module_usb_port[ port ].toggle ) module_usb_port[ port ].toggle = FALSE; else module_usb_port[ port ].toggle = TRUE;
	}

	//----------------------------------------------------------------------

	// next Transfer Descriptor
	td++;

	// terminate Transfer Descriptor
	td -> flags = MODULE_USB_DEFAULT_FLAG_terminate;
	// nothing more to do
	td -> link_pointer = EMPTY;

	// descriptor active
	td -> status = MODULE_USB_TD_STATUS_active;
	// launch Interrupt when finished
	td -> ioc = TRUE;
	// set device speed
	td -> low_speed = module_usb_port[ port ].low_speed;
	// default error counter
	td -> error_counter = STD_MAX_unsigned;

	// set Packet Identification as
	if( length_semaphore ) td -> packet_identification = MODULE_USB_TD_PACKET_IDENTIFICATION_out; else td -> packet_identification = MODULE_USB_TD_PACKET_IDENTIFICATION_in;
	// set Device Identification
	td -> device_address = module_usb_port[ port ].address_id;
	// set Endpoint Identification
	td -> endpoint = module_usb_port[ port ].endpoint_id;
	// TD semaphore
	td -> data_toggle = TRUE;	// last descriptor is always TRUE
	// no more data to be transferred
	td -> max_length = STD_MAX_unsigned;

	// nothing to transfer
	td -> buffer_pointer = EMPTY;

	//----------------------------------------------------------------------

	// insert Transfer Descriptors on Queue
	module_usb_queue_1u[ 0 ].element_link_pointer_and_flags = (uintptr_t) td_pointer;

	// wait for device
	while( td -> status & 0b10000000 );

	// remove Transfer Descriptors from Queue
	module_usb_queue_1u[ 0 ].element_link_pointer_and_flags = MODULE_USB_DEFAULT_FLAG_terminate;

	// relase Transfer Descriptors list
	kernel -> memory_release( (uintptr_t) td_pointer, TRUE );
}

void module_usb_descriptor_io( uint8_t port, uint8_t length, uintptr_t target, uint8_t flow ) {
	// prepare Transfer Descriptors area
	struct MODULE_USB_STRUCTURE_TD *td = (struct MODULE_USB_STRUCTURE_TD *) kernel -> memory_alloc_low( TRUE );

	//----------------------------------------------------------------------

	// data Transfer Descriptor
	td -> flags = MODULE_USB_DEFAULT_FLAG_data;
	// select descriptor which tells USB device where incomming data should be placed
	td -> link_pointer = (uintptr_t) (td + TRUE) >> 4;

	// descriptor active
	td -> status = MODULE_USB_TD_STATUS_active;
	// set device speed
	td -> low_speed = module_usb_port[ port ].low_speed;
	// default error counter
	td -> error_counter = STD_MAX_unsigned;

	// set Packet Identification as
	td -> packet_identification = flow;
	// set Device Identification
	td -> device_address = module_usb_port[ port ].address_id;
	// set Endpoint Identification
	td -> endpoint = module_usb_port[ port ].endpoint_id + 1;
	// TD semaphore
	td -> data_toggle = module_usb_port[ port ].toggle;
	// requested data length to be received
	td -> max_length = length - 1;

	// location of chunk of retrieved data
	td -> buffer_pointer = target;

	// next TD semaphore state
	if( module_usb_port[ port ].toggle ) module_usb_port[ port ].toggle = FALSE; else module_usb_port[ port ].toggle = TRUE;

	//----------------------------------------------------------------------

	// insert Transfer Descriptors on Queue
	module_usb_queue_8u[ 0 ].element_link_pointer_and_flags = (uintptr_t) td;

	// wait for device
	while( td -> status & 0b10000000 );

	// remove Transfer Descriptors from Queue
	module_usb_queue_8u[ 0 ].element_link_pointer_and_flags = MODULE_USB_DEFAULT_FLAG_terminate;

	// relase Transfer Descriptors list
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

	// first entry is reserved;
	module_usb_port -> flags = MODULE_USB_PORT_FLAG_reserved;

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

				// allocate area for requests data
				uintptr_t descriptor_default = kernel -> memory_alloc_low( TRUE );

				// discover every port
				// for( uint16_t j = 0; j < module_usb_controller[ i ].size_byte >> 4; j++ ) {	// thats a proper way of calculating amount of available ports inside controller, but for UHCI only
				for( uint16_t j = 0; j < 1; j++ ) {
					// port reset
					uint16_t status = EMPTY;
					if( ! (status = module_usb_port_reset( module_usb_port_count )) ) continue;	// device doesn't exist on port

					// first device number
					module_usb_port_count++;

					// register port
					module_usb_port[ module_usb_port_count ].flags = MODULE_USB_PORT_FLAG_reserved;

					// preserve port and controller IDs
					module_usb_port[ module_usb_port_count ].controller_id = i;
					module_usb_port[ module_usb_port_count ].port_id = j;
					module_usb_port[ module_usb_port_count ].max_packet_size = 0x08;	// default 8 Bytes

					// register device speed
					module_usb_port[ module_usb_port_count ].low_speed = status >> 8;

					// debug
					kernel -> log( (uint8_t *) "[USB].%u Port%u - device connected.\n", i, j );

					// prepare SETUP packet
					struct MODULE_USB_STRUCTURE_PACKET *packet = (struct MODULE_USB_STRUCTURE_PACKET *) kernel -> memory_alloc_low( TRUE );
					packet -> type		= MODULE_USB_PACKET_TYPE_direction_device_to_host;
					packet -> request	= MODULE_USB_PACKET_REQUEST_descriptor_get;
					packet -> value		= MODULE_USB_PACKET_VALUE_descriptor_type_device;
					packet -> length	= 0x08;	// default Bytes requested

					// retrieve default descriptor from device
					module_usb_descriptor( module_usb_port_count, 0x08, descriptor_default & ~KERNEL_PAGE_mirror, MODULE_USB_TD_PACKET_IDENTIFICATION_in, (uintptr_t) packet );

					// properties of device descriptor
					struct MODULE_USB_STRUCTURE_DESCRIPTOR_DEVICE *descriptor_device = (struct MODULE_USB_STRUCTURE_DESCRIPTOR_DEVICE *) descriptor_default;

					// definied class of configuration?
					if( descriptor_device -> class != MODULE_USB_DESCRIPTOR_DEVICE_CLASS_undefinied ) {
						// debug
						kernel -> log( (uint8_t *) "[USB].%u Port%u - definied device class - no support.\n", i, j );

						// next device
						continue;
					}

					// port reset
					if( ! (status = module_usb_port_reset( module_usb_port_count )) ) continue;	// device doesn't exist anymore

					// remember max packet size
					module_usb_port[ module_usb_port_count ].max_packet_size = descriptor_device -> max_packet_size;

					// acquired default descriptor length
					module_usb_port[ module_usb_port_count ].default_descriptor_length = descriptor_device -> length;

					// prepare SETUP packet
					packet -> type		= MODULE_USB_PACKET_TYPE_direction_host_to_device;
					packet -> request	= MODULE_USB_PACKET_REQUEST_address_set;
					packet -> value		= module_usb_port_count;
					packet -> length	= EMPTY;

					// set device address
					module_usb_descriptor( module_usb_port_count, EMPTY, EMPTY, EMPTY, (uintptr_t) packet );

					// assigned device address
					module_usb_port[ module_usb_port_count ].address_id = (uint64_t) ((uintptr_t) &module_usb_port[ module_usb_port_count ] - (uintptr_t) module_usb_port) / sizeof( struct MODULE_USB_STRUCTURE_PORT );

					// prepare SETUP packet
					packet -> type		= MODULE_USB_PACKET_TYPE_direction_device_to_host;
					packet -> request	= MODULE_USB_PACKET_REQUEST_descriptor_get;
					packet -> value		= MODULE_USB_PACKET_VALUE_descriptor_type_device;
					packet -> length	= module_usb_port[ module_usb_port_count ].default_descriptor_length;

					// retrieve full device descriptor
					module_usb_descriptor( module_usb_port_count, module_usb_port[ module_usb_port_count ].default_descriptor_length, descriptor_default & ~KERNEL_PAGE_mirror, MODULE_USB_TD_PACKET_IDENTIFICATION_in, (uintptr_t) packet );

					// remember amount of configuration of device
					module_usb_port[ module_usb_port_count ].configurations = descriptor_device -> configurations;

					// there is more than one configuration?
					if( module_usb_port[ module_usb_port_count ].configurations != 1 ) {
						// debug
						kernel -> log( (uint8_t *) "[USB].%u Port%u - more than 1 configuration - no support.\n", i, j );

						// next device
						continue;
					}

					// prepare CONFIG packet
					packet -> type		= MODULE_USB_PACKET_TYPE_direction_device_to_host;
					packet -> request	= MODULE_USB_PACKET_REQUEST_descriptor_get;
					packet -> value		= MODULE_USB_PACKET_VALUE_descriptor_type_configuration;
					packet -> length	= sizeof( struct MODULE_USB_STRUCTURE_DESCRIPTOR_CONFIGURATION );

					// retrieve part of first configuration properties
					module_usb_descriptor( module_usb_port_count, sizeof( struct MODULE_USB_STRUCTURE_DESCRIPTOR_CONFIGURATION ), descriptor_default & ~KERNEL_PAGE_mirror, MODULE_USB_TD_PACKET_IDENTIFICATION_in, (uintptr_t) packet );

					// properties of device descriptor
					struct MODULE_USB_STRUCTURE_DESCRIPTOR_CONFIGURATION *descriptor_configuration = (struct MODULE_USB_STRUCTURE_DESCRIPTOR_CONFIGURATION *) descriptor_default;

					// there is more than one interface?
					if( descriptor_configuration -> interface_count != 1 ) {
						// debug
						kernel -> log( (uint8_t *) "[USB].%u Port%u - more than 1 interface - no support.\n", i, j );

						// next device
						continue;
					}

					// retrieve first configuration properties (full)
					packet -> length = descriptor_configuration -> total_length;
					module_usb_descriptor( module_usb_port_count, descriptor_configuration -> total_length, descriptor_default & ~KERNEL_PAGE_mirror, MODULE_USB_TD_PACKET_IDENTIFICATION_in, (uintptr_t) packet );

					// prepare CONFIG packet
					packet -> type		= MODULE_USB_PACKET_TYPE_direction_host_to_device;
					packet -> request	= MODULE_USB_PACKET_REQUEST_configuration_set;
					packet -> value		= descriptor_configuration -> config_value;
					packet -> length	= EMPTY;

					// select first configuration as default
					module_usb_descriptor( module_usb_port_count, EMPTY, EMPTY, EMPTY, (uintptr_t) packet );

					// prepare pointer to first interface and endpoint
					struct MODULE_USB_STRUCTURE_DESCRIPTOR_INTERFACE *descriptor_interface = EMPTY;
					struct MODULE_USB_STRUCTURE_DESCRIPTOR_ENDPOINT *descriptor_endpoint = EMPTY;

					// locate first interface of device configuration
					uint16_t k = sizeof( struct MODULE_USB_STRUCTURE_DESCRIPTOR_CONFIGURATION );
					while( k < descriptor_configuration -> total_length ) {
						// default properties of descriptor
						struct MODULE_USB_STRUCTURE_DESCRIPTOR_DEFAULT *descriptor_find = (struct MODULE_USB_STRUCTURE_DESCRIPTOR_DEFAULT *) ((uintptr_t) descriptor_configuration + k);

						// Interface?
						if( ! descriptor_interface && descriptor_find -> type == (MODULE_USB_PACKET_VALUE_descriptor_type_interface >> STD_MOVE_BYTE) ) descriptor_interface = (struct MODULE_USB_STRUCTURE_DESCRIPTOR_INTERFACE *) descriptor_find;	// found

						// Endpoint?
						if( ! descriptor_endpoint && descriptor_find -> type == (MODULE_USB_PACKET_VALUE_descriptor_type_endpoint >> STD_MOVE_BYTE) ) descriptor_endpoint = (struct MODULE_USB_STRUCTURE_DESCRIPTOR_ENDPOINT *) descriptor_find;	// found

						// move default further of configuration descriptor
						k += descriptor_find -> length;
					}

					// HID device?
					if( descriptor_interface -> class != MODULE_USB_DESCRIPTOR_INTERFACE_CLASS_hid ) continue;	// no

					// available Boot Interface?
					if( descriptor_interface -> subclass != MODULE_USB_DESCRIPTOR_INTERFACE_SUBCLASS_boot_interface ) continue;	// no

					// device type of Keyboard?
					if( descriptor_interface -> protocol != MODULE_USB_DESCRIPTOR_INTERFACE_PROTOCOL_keyboard ) continue;	// no

					// remember endpoint value
					module_usb_port[ module_usb_port_count ].endpoint_id = (descriptor_endpoint -> address & 0x1111) - 1;

					// prepare IDLE packet
					packet -> type		= MODULE_USB_PACKET_TYPE_direction_host_to_device | MODULE_USB_PACKET_TYPE_subtype_class | MODULE_USB_PACKET_TYPE_recipient_interface;
					packet -> request	= MODULE_USB_PACKET_REQUEST_idle_set;
					packet -> value		= EMPTY;	// Indefinitiely and All Reports
					packet -> index		= descriptor_interface -> interface_id;
					packet -> length	= EMPTY;

					// set interface as IDLE
					module_usb_descriptor( module_usb_port_count, EMPTY, EMPTY, EMPTY, (uintptr_t) packet );

					// TD semaphore, reset
					module_usb_port[ module_usb_port_count ].toggle = FALSE;

					// debug
					uint8_t *test = (uint8_t *) descriptor_default;
					kernel -> memory_clean( (uint64_t *) descriptor_default, TRUE );
					while( TRUE ) {
						module_usb_descriptor_io( module_usb_port_count, 0x08, descriptor_default & ~KERNEL_PAGE_mirror, MODULE_USB_TD_PACKET_IDENTIFICATION_in );
						if( test[ 2 ] && test[ 2 ] < 0x28 ) {
							for( uint8_t q = 2; q < 8; q++ ) if( test[ q ] )
								// in first free space in keyboard buffer
								for( uint8_t i = 0; i < 8; i++ )
									// save key code
									if( ! kernel -> device_keyboard[ i ] ) { kernel -> device_keyboard[ i ] = module_usb_keyboard_matrix_low[ test[ q ] ]; test[ q ] = EMPTY; break; }
						}
					}
				}
			}
		}
	}

	// hold the door
	while( TRUE ) kernel -> time_sleep( TRUE );
}