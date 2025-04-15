/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions of kernel
	//----------------------------------------------------------------------
	#include	"../kernel/apic.h"
	#include	"../kernel/idt.h"
	#include	"../kernel/io_apic.h"
	#include	"../kernel/page.h"
	// --- always at end
	#include	"../kernel/config.h"
	//======================================================================

	//----------------------------------------------------------------------
	// Build-in driver variables, structures, definitions
	//----------------------------------------------------------------------
	#include	"../kernel/driver/pci.h"
	#include	"../kernel/driver/port.h"
	//======================================================================

	//----------------------------------------------------------------------
	// Build-in driver routines, procedures
	//----------------------------------------------------------------------
	#include	"../kernel/driver/pci.c"
	#include	"../kernel/driver/port.c"
	//======================================================================

	//----------------------------------------------------------------------
	// variables, structures, definitions of module
	//----------------------------------------------------------------------
	#include	"./usb/config.h"
	#include	"./usb/uhci.h"
	//======================================================================

	//----------------------------------------------------------------------
	// variables of module
	//----------------------------------------------------------------------
	#include	"./usb/data.c"
	#include	"./usb/uhci.c"
	//======================================================================

// uint16_t module_usb_hid_keyboard_matrix( uint8_t id ) {
// 	// translate key id, default: low matrix
// 	uint16_t key = module_usb_keyboard_matrix_low[ id ];

// 	// high matrix?
// 	if( module_usb_keyboard_cache[ 0 ] & (MODULE_USB_HID_KEYBOARD_KEY_CODE_SHIFT_LEFT | MODULE_USB_HID_KEYBOARD_KEY_CODE_SHIFT_RIGHT) )
// 		// yes
// 		key = module_usb_keyboard_matrix_high[ id ];

// 	// return key from matrix
// 	return key;
// }

// void module_usb_hid_keyboard_save( uint16_t key ) {
// 	// in first free space in keyboard buffer
// 	for( uint8_t c = 0; c < 8; c++ )
// 		// save character
// 		if( ! kernel -> device_keyboard[ c ] ) { kernel -> device_keyboard[ c ] = key; break; }
// }

// void module_usb_hid_keyboard( void ) {
// 	// prepare keyboard input cache
// 	uint8_t *cache = (uint8_t *) kernel -> memory_alloc_low( TRUE );

// 	// receive keys
// 	while( TRUE ) {
// 		// from any available keyboard
// 		for( uint8_t p = 0; p < MODULE_USB_PORT_limit; p++ ) {
// 			// registered controller with definied protocol?
// 			if( module_usb_port[ p ].type != MODULE_USB_DEVICE_TYPE_HID_KEYBOARD ) continue;	// no

// 			// clean up cache
// 			kernel -> memory_clean( (uint64_t *) cache, TRUE );

// 			// create input Transfer Descriptor
// 			uint8_t status = module_usb_uhci_descriptor_io( (struct MODULE_USB_STRUCTURE_PORT *) &module_usb_port[ p ], 0x08, (uintptr_t) cache & ~KERNEL_MEMORY_mirror, MODULE_USB_UHCI_TD_PACKET_IDENTIFICATION_in, 16 );

// 			// if something bad hapenned
// 			if( status ) continue;	// ignore keys

// 			// check special key
// 			if( *cache != module_usb_keyboard_cache[ 0 ] ) {
// 				// key update
// 				uint16_t key;

// 				// CTRL left
// 				if( module_usb_keyboard_cache[ 0 ] != (*cache & MODULE_USB_HID_KEYBOARD_KEY_CODE_CTRL_LEFT) ) {
// 					// default state
// 					key = STD_KEY_CTRL_LEFT;

// 					// if released
// 					if( ! (*cache & MODULE_USB_HID_KEYBOARD_KEY_CODE_CTRL_LEFT) ) key |= STD_KEY_RELEASE;

// 					// insert key into cache
// 					module_usb_hid_keyboard_save( key );
// 				}

// 				// SHIFT left
// 				if( module_usb_keyboard_cache[ 0 ] != (*cache & MODULE_USB_HID_KEYBOARD_KEY_CODE_SHIFT_LEFT) ) {
// 					// default state
// 					key = STD_KEY_SHIFT_LEFT;

// 					// if released
// 					if( ! (*cache & MODULE_USB_HID_KEYBOARD_KEY_CODE_SHIFT_LEFT) ) key |= STD_KEY_RELEASE;
					
// 					// insert key into cache
// 					module_usb_hid_keyboard_save( key );
// 				}

// 				// ALT left
// 				if( module_usb_keyboard_cache[ 0 ] != (*cache & MODULE_USB_HID_KEYBOARD_KEY_CODE_ALT_LEFT) ) {
// 					// default state
// 					key = STD_KEY_ALT_LEFT;

// 					// if released
// 					if( ! (*cache & MODULE_USB_HID_KEYBOARD_KEY_CODE_ALT_LEFT) ) key |= STD_KEY_RELEASE;
					
// 					// insert key into cache
// 					module_usb_hid_keyboard_save( key );
// 				}

// 				// MENU left
// 				if( module_usb_keyboard_cache[ 0 ] != (*cache & MODULE_USB_HID_KEYBOARD_KEY_CODE_MENU_LEFT) ) {
// 					// default state
// 					key = STD_KEY_MENU;

// 					// if released
// 					if( ! (*cache & MODULE_USB_HID_KEYBOARD_KEY_CODE_MENU_LEFT) ) key |= STD_KEY_RELEASE;
					
// 					// insert key into cache
// 					module_usb_hid_keyboard_save( key );
// 				}

// 				// preserve special key state
// 				module_usb_keyboard_cache[ 0 ] = *cache;
// 			}

// 			// release key
// 			for( uint8_t i = 2; i < 8; i++ ) {
// 				// ignore empty fields
// 				if( ! module_usb_keyboard_cache[ i ] ) continue;

// 				// released by default
// 				uint8_t released = TRUE;

// 				// search for released key
// 				for( uint8_t j = 2; j < 8; j++ ) if( module_usb_keyboard_cache[ i ] == cache[ j ] ) released = FALSE;

// 				// released a key?
// 				if( ! released ) continue;	// no

// 				// insert key into kernel cache
// 				module_usb_hid_keyboard_save( module_usb_hid_keyboard_matrix( module_usb_keyboard_cache[ i ] ) | STD_KEY_RELEASE );
// 			}


// 			// press key
// 			for( uint8_t i = 2; i < 8; i++ ) {
// 				// ignore empty fields
// 				if( ! cache[ i ] ) continue;

// 				// not pressed by default
// 				uint8_t pressed = TRUE;

// 				// search for pressed key
// 				for( uint8_t j = 2; j < 8; j++ ) if( cache[ i ] == module_usb_keyboard_cache[ j ] ) pressed = FALSE;

// 				// pressed a key?
// 				if( ! pressed ) continue;	// no

// 				// insert key into kernel cache
// 				module_usb_hid_keyboard_save( module_usb_hid_keyboard_matrix( cache[ i ] ) );
// 			}

// 			// update local key cache
// 			for( uint8_t i = 2; i < 8; i++ ) module_usb_keyboard_cache[ i ] = cache[ i ];
// 		}

// 		// release CPU time
// 		kernel -> time_sleep( TRUE );
// 	}
// }

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
			module_usb_uhci_descriptor_io( (struct MODULE_USB_STRUCTURE_PORT *) &module_usb_port[ p ], 0x04, (uintptr_t) cache & ~KERNEL_MEMORY_mirror, MODULE_USB_UHCI_TD_PACKET_IDENTIFICATION_in, 8 );

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

		// release CPU time
		kernel -> time_sleep( TRUE );
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

void _entry( uintptr_t kernel_ptr ) {
	// preserve kernel structure pointer
	kernel = (struct KERNEL *) kernel_ptr;

	// assign area for discovered USB controllers
	module_usb_controller = (struct MODULE_USB_STRUCTURE_CONTROLLER *) kernel -> memory_alloc_low( MACRO_PAGE_ALIGN_UP( sizeof( struct MODULE_USB_STRUCTURE_CONTROLLER ) * MODULE_USB_CONTROLLER_limit ) >> STD_SHIFT_PAGE );

	// assign area for discovered USB ports
	module_usb_port = (struct MODULE_USB_STRUCTURE_PORT *) kernel -> memory_alloc_low( MACRO_PAGE_ALIGN_UP( sizeof( struct MODULE_USB_STRUCTURE_PORT ) * MODULE_USB_PORT_limit ) >> STD_SHIFT_PAGE );

	// check "every" bus;device;function of PCI controller
	for( uint16_t b = INIT; b < 32; b++ )
		for( uint8_t d = INIT; d < 255; d++ )
			for( uint8_t f = INIT; f < 8; f++ ) {
				// PCI properties
				module_usb_controller[ module_usb_controller_limit ].pci.result		= EMPTY;
				module_usb_controller[ module_usb_controller_limit ].pci.bus		= b;
				module_usb_controller[ module_usb_controller_limit ].pci.device		= d;
				module_usb_controller[ module_usb_controller_limit ].pci.function	= f;

				// retrieved class-subclass
				uint32_t class_subclass = driver_pci_read( module_usb_controller[ module_usb_controller_limit ].pci, DRIVER_PCI_REGISTER_class_and_subclass ) >> 8;

				// localization of base address inside PCI, default values
				uint8_t bar_low = DRIVER_PCI_REGISTER_bar0;
				uint8_t bar_high = DRIVER_PCI_REGISTER_bar1;

				// if found
				if( class_subclass == DRIVER_PCI_CLASS_SUBCLASS_usb_uhci ) {
					// debug
					kernel -> serial( (uint8_t *) "[USB].%u PCI %2X:%2X.%u - USB controller found. (UHCI - Universal Host Controller Interface)\n", module_usb_controller_limit, module_usb_controller[ module_usb_controller_limit ].pci.bus, module_usb_controller[ module_usb_controller_limit ].pci.device, module_usb_controller[ module_usb_controller_limit ].pci.function );

					// set type
					module_usb_controller[ module_usb_controller_limit ].type = MODULE_USB_CONTROLLER_TYPE_UHCI;

					// disable BIOS legacy support
					driver_pci_write( module_usb_controller[ module_usb_controller_limit ].pci, 0xC0, 0x8F00 );

					// UHCI uses different values for BARs
					bar_low = DRIVER_PCI_REGISTER_bar4;
					bar_high = DRIVER_PCI_REGISTER_bar5;
				}

	// 			// if found
	// 			if( class_subclass == DRIVER_PCI_CLASS_SUBCLASS_usb_ohci ) {
	// 				// debug
	// 				// kernel -> serial( (uint8_t *) "[USB].%u PCI %2X:%2X.%u - USB controller found. (OHCI - Open Host Controller Interface)\n", module_usb_controller_limit, module_usb_controller[ module_usb_controller_limit ].pci.bus, module_usb_controller[ module_usb_controller_limit ].pci.device, module_usb_controller[ module_usb_controller_limit ].pci.function );

	// 				// set type
	// 				module_usb_controller[ module_usb_controller_limit ].type = MODULE_USB_CONTROLLER_TYPE_OHCI;
	// 			}

	// 			// if found
	// 			if( class_subclass == DRIVER_PCI_CLASS_SUBCLASS_usb_ehci ) {
	// 				// debug
	// 				// kernel -> serial( (uint8_t *) "[USB].%u PCI %2X:%2X.%u - USB controller found. (EHCI - Enchanced Host Controller Interface)\n", module_usb_controller_limit, module_usb_controller[ module_usb_controller_limit ].pci.bus, module_usb_controller[ module_usb_controller_limit ].pci.device, module_usb_controller[ module_usb_controller_limit ].pci.function );

	// 				// set type
	// 				module_usb_controller[ module_usb_controller_limit ].type = MODULE_USB_CONTROLLER_TYPE_EHCI;
	// 			}

	// 			// if found
	// 			if( class_subclass == DRIVER_PCI_CLASS_SUBCLASS_usb_xhci ) {
	// 				// debug
	// 				// kernel -> serial( (uint8_t *) "[USB].%u PCI %2X:%2X.%u - USB controller found. (xHCI - eXtensible Host Controller Interface)\n", module_usb_controller_limit, module_usb_controller[ module_usb_controller_limit ].pci.bus, module_usb_controller[ module_usb_controller_limit ].pci.device, module_usb_controller[ module_usb_controller_limit ].pci.function );

	// 				// set type
	// 				module_usb_controller[ module_usb_controller_limit ].type = MODULE_USB_CONTROLLER_TYPE_xHCI;
	// 			}

				// controller recognized?
				if( ! module_usb_controller[ module_usb_controller_limit ].type ) continue;	// no

				// set base address of controller, irq and mmio semaphore
				module_usb_controller[ module_usb_controller_limit ].base_address = driver_pci_read( module_usb_controller[ module_usb_controller_limit ].pci, bar_low );
				module_usb_controller[ module_usb_controller_limit ].irq_pin_and_line = driver_pci_read( module_usb_controller[ module_usb_controller_limit ].pci, DRIVER_PCI_REGISTER_irq ) & STD_MASK_word;
				module_usb_controller[ module_usb_controller_limit ].mmio_semaphore = FALSE;	// by default

				// detect length of port area
				driver_pci_write( module_usb_controller[ module_usb_controller_limit ].pci, bar_low, 0xFFFFFFFF );
				module_usb_controller[ module_usb_controller_limit ].limit = ( ~( driver_pci_read( module_usb_controller[ module_usb_controller_limit ].pci, bar_low ) & ~1 ) + 1 ) >> STD_SHIFT_16;
		
				// restore original value
				driver_pci_write( module_usb_controller[ module_usb_controller_limit ].pci, bar_low, module_usb_controller[ module_usb_controller_limit ].base_address );

				// undefinied limit?
				if( ! module_usb_controller[ module_usb_controller_limit ].limit ) module_usb_controller[ module_usb_controller_limit ].limit = 2;	// default length

				// MMIO type of address?
				if( ! (module_usb_controller[ module_usb_controller_limit ].base_address & TRUE) ) {
					// yes
					module_usb_controller[ module_usb_controller_limit ].mmio_semaphore = TRUE;

					// 64 bit address?
					if( (module_usb_controller[ module_usb_controller_limit ].base_address & ~STD_PAGE_mask) == 0x04 )
						// retrieve higher address value
						module_usb_controller[ module_usb_controller_limit ].base_address |= (uint64_t) driver_pci_read( module_usb_controller[ module_usb_controller_limit ].pci, bar_high ) << STD_MOVE_DWORD;

					// map MMIO controller area
					kernel -> page_map( kernel -> page_base_address, MACRO_PAGE_ALIGN_DOWN( module_usb_controller[ module_usb_controller_limit ].base_address ), MACRO_PAGE_ALIGN_DOWN( module_usb_controller[ module_usb_controller_limit ].base_address ) | KERNEL_MEMORY_mirror, MACRO_PAGE_ALIGN_UP( (module_usb_controller[ module_usb_controller_limit ].base_address & ~STD_PAGE_mask) + offsetof( struct MODULE_USB_STRUCTURE_UHCI_REGISTER, port ) + (module_usb_controller[ module_usb_controller_limit ].limit << STD_SHIFT_16) ) >> STD_SHIFT_PAGE, KERNEL_PAGE_FLAG_present | KERNEL_PAGE_FLAG_write );
				}

				// reset flags
				module_usb_controller[ module_usb_controller_limit ].base_address &= ~STD_BIT_CONTROL_WORD_bit;

				// convert base address to logical
				module_usb_controller[ module_usb_controller_limit ].base_address |= KERNEL_MEMORY_mirror;

				// controller registered
				module_usb_controller_limit++;
			}

	// // initialize xHCI controllers
	// for( uint8_t c = 0; c < module_usb_controller_limit; c++ ) if( module_usb_controller[ c ].type == MODULE_USB_CONTROLLER_TYPE_xHCI ) module_usb_xhci_init( c );

	// // initialize EHCI controllers
	// for( uint8_t c = 0; c < module_usb_controller_limit; c++ ) if( module_usb_controller[ c ].type == MODULE_USB_CONTROLLER_TYPE_EHCI ) module_usb_ehci_init( c );

	// initialize controllers
	for( uint8_t c = INIT; c < module_usb_controller_limit; c++ )
		// depending of controller type
		switch( module_usb_controller[ c ].type ) {
			case MODULE_USB_CONTROLLER_TYPE_UHCI: { module_usb_uhci_init( c ); break; }

	// 		// initialize controller
	// 		case MODULE_USB_CONTROLLER_TYPE_OHCI: {
	// 			// initialize OHCI Controller
	// 			module_usb_ohci_init( c );

	// 			// done
	// 			break;
	// 		}
		}

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
					kernel -> serial( (uint8_t *) "[USB].%u Port%u - No device.\n", c, p );

					// next device
					continue;
				}

				// debug
				kernel -> serial( (uint8_t *) "[USB].%u Port%u - Attached, ", c, p );

				// debug
				if( port -> low_speed ) kernel -> serial( (uint8_t *) "Low-Speed\n" );
				else kernel -> serial( (uint8_t *) "Full-Speed\n" );
			}
		}
	}

	// enable mouse thread
	uint8_t module_usb_string_hid_mouse[] = "usb.ko - mouse";
	kernel -> module_thread( (uintptr_t) &module_usb_hid_mouse, (uint8_t *) &module_usb_string_hid_mouse, sizeof( module_usb_string_hid_mouse ) - 1 );

	// // enable keyboard thread
	// uint8_t module_usb_string_hid_keyboard[] = "usb.ko - keyboard";
	// kernel -> module_thread( (uintptr_t) &module_usb_hid_keyboard, (uint8_t *) &module_usb_string_hid_keyboard, sizeof( module_usb_string_hid_keyboard ) - 1 );

	// hold the door
	while( TRUE ) kernel -> time_sleep( TRUE );
}
