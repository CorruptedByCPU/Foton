/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// variables, structures, definitions of kernel
	//----------------------------------------------------------------------
	#include	"../kernel/config.h"
	#include	"../kernel/idt.h"
	#include	"../kernel/io_apic.h"
	#include	"../kernel/lapic.h"
	//----------------------------------------------------------------------
	// drivers
	//----------------------------------------------------------------------
	#include	"../kernel/driver/port.h"
	#include	"../kernel/driver/port.c"
	//----------------------------------------------------------------------
	// variables, structures, definitions of module
	//----------------------------------------------------------------------
	#include	"./ps2/config.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./ps2/data.c"

void driver_ps2_check_read( void ) {
	// wait for output data
	while( ! (driver_port_in_byte( DRIVER_PS2_PORT_COMMAND_OR_STATUS ) & DRIVER_PS2_STATUS_output) );
}

void driver_ps2_check_write( void ) {
	// wait for input data
	while( driver_port_in_byte( DRIVER_PS2_PORT_COMMAND_OR_STATUS ) & DRIVER_PS2_STATUS_input );
}

void driver_ps2_command( uint8_t command ) {
	// wait for controller to be ready to accept command
	driver_ps2_check_write();

	// send command
	driver_port_out_byte( DRIVER_PS2_PORT_COMMAND_OR_STATUS, command );
}

uint8_t driver_ps2_data_read( void ) {
	// wait for controller to be ready to accept answer/data
	driver_ps2_check_read();

	// receive answer/data
	return driver_port_in_byte( DRIVER_PS2_PORT_DATA );
}

void driver_ps2_data_write( uint8_t data ) {
	// wait for controller to be ready to accept command/data
	driver_ps2_check_write();

	// send data
	driver_port_out_byte( DRIVER_PS2_PORT_DATA, data );
}

void driver_ps2_drain( void ) {
	// flush PS2 controller output buffer
	while( driver_port_in_byte( DRIVER_PS2_PORT_COMMAND_OR_STATUS ) & DRIVER_PS2_STATUS_output )
		// drop data from PS2 controller
		driver_port_in_byte( DRIVER_PS2_PORT_DATA );
	
	// there is nothig left, good
}

__attribute__(( no_caller_saved_registers ))
void driver_ps2_mouse( void ) {
}

__attribute__(( no_caller_saved_registers ))
void driver_ps2_keyboard( void ) {
	// get key code
	volatile uint16_t key = driver_ps2_data_read();

	// perform operation depending on opcode
	switch( key ) {
		// controller started sequence?
		case DRIVER_PS2_KEYBOARD_sequence:
			// save sequence type
			driver_ps2_scancode = key << STD_SHIFT_8;
			break;

		// controller started alternate sequence?
		case DRIVER_PS2_KEYBOARD_sequence_alternative:
			// zachowaj typ sekwencji
			driver_ps2_scancode = key << STD_SHIFT_8;
			break;

		// controller doesn't start sequence?
		default:
			// complete sequence?
			if( driver_ps2_scancode ) {
				// compose key code
				key |= driver_ps2_scancode;

				// sequence processed
				driver_ps2_scancode = EMPTY;
			}
	}

	// tell APIC of current logical processor that hardware interrupt is being handled, propely
	kernel -> lapic_base_address -> eoi = EMPTY;
}

void driver_ps2_init( void ) {
	// drain PS2 controller buffer
	driver_ps2_drain();

	// retrieve PS2 controller configuration
	driver_ps2_command( DRIVER_PS2_COMMAND_CONFIGURATION_GET );
	uint8_t configuration = driver_ps2_data_read();

	// turn on interrupts and clock on device: mouse
	driver_ps2_command( DRIVER_PS2_COMMAND_CONFIGURATION_SET );
	driver_ps2_data_write( (configuration | DRIVER_PS2_CONFIGURATION_PORT_SECOND_INTERRUPT) & ~DRIVER_PS2_CONFIGURATION_PORT_SECOND_CLOCK );

	// send a RESET command to device: mouse
	driver_ps2_command( DRIVER_PS2_COMMAND_PORT_SECOND );
	driver_ps2_data_write( DRIVER_PS2_DEVICE_RESET );

	// command accepted?
	if( driver_ps2_data_read() == DRIVER_PS2_ANSWER_ACKNOWLEDGED ) {
		// device is working properly?
		if( driver_ps2_data_read() == DRIVER_PS2_ANSWER_SELF_TEST_SUCCESS ) {
			// get device ID
			driver_ps2_mouse_type = driver_ps2_data_read();

			// send SET DEFAULT command to device: mouse
			driver_ps2_command( DRIVER_PS2_COMMAND_PORT_SECOND );
			driver_ps2_data_write( DRIVER_PS2_DEVICE_SET_DEFAULT );

			// command accepted?
			if( driver_ps2_data_read() == DRIVER_PS2_ANSWER_ACKNOWLEDGED ) {
				// send PACKETS ENABLE command to device: mouse
				driver_ps2_command( DRIVER_PS2_COMMAND_PORT_SECOND );
				driver_ps2_data_write( DRIVER_PS2_DEVICE_PACKETS_ENABLE );

				// command accepted?
				if( driver_ps2_data_read() == DRIVER_PS2_ANSWER_ACKNOWLEDGED ) {
					// show information about mouse device
					kernel -> log( (uint8_t *) "[PS2 module] Mouse connected.\n" );

					// connect PS2 controller interrupt handler for device: mouse
					kernel -> idt_mount( KERNEL_IDT_IRQ_offset + DRIVER_PS2_MOUSE_IRQ_number, KERNEL_IDT_TYPE_irq, (uintptr_t) driver_ps2_mouse_entry );

					// connect interrupt vector from IDT table in IOAPIC controller
					kernel -> io_apic_connect( KERNEL_IDT_IRQ_offset + DRIVER_PS2_MOUSE_IRQ_number, DRIVER_PS2_MOUSE_IO_APIC_register );
				}
			}
		}
	}

	// connect PS2 controller interrupt handler for device: keyboard
	kernel -> idt_mount( KERNEL_IDT_IRQ_offset + DRIVER_PS2_KEYBOARD_IRQ_number, KERNEL_IDT_TYPE_irq, (uint64_t) driver_ps2_keyboard_entry );

	// connect interrupt vector from IDT table in IOAPIC controller
	kernel -> io_apic_connect( KERNEL_IDT_IRQ_offset + DRIVER_PS2_KEYBOARD_IRQ_number, DRIVER_PS2_KEYBOARD_IO_APIC_register );

	// show information about mouse device
	kernel -> log( (uint8_t *) "[PS2 module] Keyboard connected.\n" );
}

void _entry( uintptr_t kernel_ptr ) {
	// preserve kernel structure pointer
	kernel = (struct KERNEL *) kernel_ptr;

	// initialize available PS2 devices
	driver_ps2_init();

	// hold the door
	while( TRUE );
}
