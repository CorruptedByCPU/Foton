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

void module_ps2_check_read( void ) {
	// wait for output data
	while( ! (driver_port_in_byte( MODULE_PS2_PORT_COMMAND_OR_STATUS ) & MODULE_PS2_STATUS_output) );
}

void module_ps2_check_write( void ) {
	// wait for input data
	while( driver_port_in_byte( MODULE_PS2_PORT_COMMAND_OR_STATUS ) & MODULE_PS2_STATUS_input );
}

void module_ps2_command( uint8_t command ) {
	// wait for controller to be ready to accept command
	module_ps2_check_write();

	// send command
	driver_port_out_byte( MODULE_PS2_PORT_COMMAND_OR_STATUS, command );
}

uint8_t module_ps2_data_read( void ) {
	// wait for controller to be ready to accept answer/data
	module_ps2_check_read();

	// receive answer/data
	return driver_port_in_byte( MODULE_PS2_PORT_DATA );
}

uint8_t	module_ps2_status_read( void ) {
	// receive status
	return driver_port_in_byte( MODULE_PS2_PORT_COMMAND_OR_STATUS );
}

void module_ps2_data_write( uint8_t data ) {
	// wait for controller to be ready to accept command/data
	module_ps2_check_write();

	// send data
	driver_port_out_byte( MODULE_PS2_PORT_DATA, data );
}

void module_ps2_drain( void ) {
	// flush PS2 controller output buffer
	while( driver_port_in_byte( MODULE_PS2_PORT_COMMAND_OR_STATUS ) & MODULE_PS2_STATUS_output )
		// drop data from PS2 controller
		driver_port_in_byte( MODULE_PS2_PORT_DATA );
	
	// there is nothig left, good
}

__attribute__(( no_caller_saved_registers ))
void module_ps2_mouse( void ) {
	// data from second controller port?
	if( driver_port_in_byte( MODULE_PS2_PORT_COMMAND_OR_STATUS ) & MODULE_PS2_STATUS_output_second ) {
		// retrieve package from PS2 controller
		int8_t package = module_ps2_data_read();

		// perform operation depending on number of package
		switch( module_ps2_mouse_package_id ) {
			case 0: {
				// status package contains ALWAYS ON bit?
				if( ! (package & MODULE_PS2_MOUSE_PACKET_ALWAYS_ONE ) ) break;	// no

				// overflow on X axis?
				if( package & MODULE_PS2_MOUSE_PACKET_OVERFLOW_x ) break;	// yes

				// overflow on Y axis?
				if( package & MODULE_PS2_MOUSE_PACKET_OVERFLOW_y ) break;	// yes

				// save device status: mouse
				kernel -> device_mouse_status = package;

				// package handled from given interrupt
				module_ps2_mouse_package_id++;

				// package parsed
				break;
			}

			case 1: {
				// calculate relative movement at X axis
				int8_t rx = package - ((kernel -> device_mouse_status << 4) & 0x100);

				// overflow from left?
				if( kernel -> device_mouse_x + rx < 0 ) kernel -> device_mouse_x = EMPTY;

				// overflow from right?
				else if( kernel -> device_mouse_x + rx > kernel -> framebuffer_width_pixel - 1 ) kernel -> device_mouse_x = kernel -> framebuffer_width_pixel - 1;
				
				// compound new position
				else kernel -> device_mouse_x += rx;

				// package handled from given interrupt
				module_ps2_mouse_package_id++;

				// package parsed
				break;
			}

			case 2: {
				// calculate relative movement at Y axis
				int8_t ry = ~(package - ((kernel -> device_mouse_status << 3) & 0x100)) + 1;

				// overflow from top?
				if( kernel -> device_mouse_y + ry < 0 ) kernel -> device_mouse_y = EMPTY;

				// overflow from bottom?
				else if( kernel -> device_mouse_y + ry > kernel -> framebuffer_height_pixel - 1 ) kernel -> device_mouse_y = kernel -> framebuffer_height_pixel - 1;

				// compound new position
				else kernel -> device_mouse_y += ry;

				// package handled from given interrupt
				module_ps2_mouse_package_id = EMPTY;

				// package parsed
				break;
			}
		}
	}

	// tell APIC of current logical processor that hardware interrupt was handled, propely
	kernel -> lapic_base_address -> eoi = EMPTY;
}

__attribute__(( no_caller_saved_registers ))
void module_ps2_keyboard( void ) {
	// get key code
	module_ps2_scancode |= module_ps2_data_read();

	// perform operation depending on opcode

	// controller started sequence?
	if( module_ps2_scancode == MODULE_PS2_KEYBOARD_sequence ) {
		// save sequence type
		module_ps2_scancode = 0xE000;

		// tell APIC of current logical processor that hardware interrupt was handled, propely
		kernel -> lapic_base_address -> eoi = EMPTY;

		// end of interrupt
		return;
	}

	// controller started alternate sequence?
	if( module_ps2_scancode == MODULE_PS2_KEYBOARD_sequence_alternative ) {
		// zachowaj typ sekwencji
		module_ps2_scancode = 0xE100;

		// tell APIC of current logical processor that hardware interrupt was handled, propely
		kernel -> lapic_base_address -> eoi = EMPTY;

		// end of interrupt
		return;
	}

	// complete sequence?
	if( module_ps2_scancode < 0xE000 ) {
		// key code not in matrix?
		if( module_ps2_scancode >= 0x80 ) {
			// get ASCII code for key from matrix
			if( ! module_ps2_keyboard_matrix ) module_ps2_scancode = module_ps2_keyboard_matrix_low[ module_ps2_scancode - 0x80 ];
			else module_ps2_scancode = module_ps2_keyboard_matrix_high[ module_ps2_scancode - 0x80 ];

			// correct key code
			module_ps2_scancode += 0x80;
		} else
			// get ASCII code for key from matrix
			if( ! module_ps2_keyboard_matrix ) module_ps2_scancode = module_ps2_keyboard_matrix_low[ module_ps2_scancode ];
			else module_ps2_scancode = module_ps2_keyboard_matrix_high[ module_ps2_scancode ];
	}

	// SHIFT or CAPSLOCK key?
	if( module_ps2_scancode == STD_KEY_CAPSLOCK || module_ps2_scancode == STD_KEY_SHIFT_LEFT || module_ps2_scancode == STD_KEY_SHIFT_RIGHT ) {
		if( module_ps2_keyboard_matrix ) module_ps2_keyboard_matrix = FALSE;
		else module_ps2_keyboard_matrix = TRUE;
	} else if( module_ps2_scancode == STD_KEY_SHIFT_LEFT + 0x80 || module_ps2_scancode == STD_KEY_SHIFT_RIGHT + 0x80 ) {
		if( module_ps2_keyboard_matrix ) module_ps2_keyboard_matrix = FALSE;
		else module_ps2_keyboard_matrix = TRUE;
	}

	// in first free space in keyboard buffer
	for( uint8_t i = 0; i < MODULE_PS2_CACHE_limit; i++ )
		// save key code
		if( ! kernel -> device_keyboard[ i ] ) { kernel -> device_keyboard[ i ] = module_ps2_scancode; break; }

	// key processed
	module_ps2_scancode = EMPTY;

	// tell APIC of current logical processor that hardware interrupt was handled, propely
	kernel -> lapic_base_address -> eoi = EMPTY;
}

void module_ps2_init( void ) {
	// drain PS2 controller buffer
	module_ps2_drain();

	// retrieve PS2 controller configuration
	module_ps2_command( MODULE_PS2_COMMAND_CONFIGURATION_GET );
	uint8_t configuration = module_ps2_data_read();

	// turn on interrupts and clock on device: mouse
	module_ps2_command( MODULE_PS2_COMMAND_CONFIGURATION_SET );
	module_ps2_data_write( (configuration | MODULE_PS2_CONFIGURATION_PORT_SECOND_INTERRUPT) & ~MODULE_PS2_CONFIGURATION_PORT_SECOND_CLOCK );

	// send a RESET command to device: mouse
	module_ps2_command( MODULE_PS2_COMMAND_PORT_SECOND );
	module_ps2_data_write( MODULE_PS2_DEVICE_RESET );

	// command accepted?
	if( module_ps2_data_read() == MODULE_PS2_ANSWER_ACKNOWLEDGED ) {
		// device is working properly?
		if( module_ps2_data_read() == MODULE_PS2_ANSWER_SELF_TEST_SUCCESS ) {
			// get device ID
			module_ps2_mouse_type = module_ps2_data_read();

			// send SET DEFAULT command to device: mouse
			module_ps2_command( MODULE_PS2_COMMAND_PORT_SECOND );
			module_ps2_data_write( MODULE_PS2_DEVICE_SET_DEFAULT );

			// command accepted?
			if( module_ps2_data_read() == MODULE_PS2_ANSWER_ACKNOWLEDGED ) {
				// send PACKETS ENABLE command to device: mouse
				module_ps2_command( MODULE_PS2_COMMAND_PORT_SECOND );
				module_ps2_data_write( MODULE_PS2_DEVICE_PACKETS_ENABLE );

				// command accepted?
				if( module_ps2_data_read() == MODULE_PS2_ANSWER_ACKNOWLEDGED ) {
					// connect PS2 controller interrupt handler for device: mouse
					kernel -> idt_mount( KERNEL_IDT_IRQ_offset + MODULE_PS2_MOUSE_IRQ_number, KERNEL_IDT_TYPE_irq, (uintptr_t) module_ps2_mouse_entry );

					// connect interrupt vector from IDT table in IOAPIC controller
					kernel -> io_apic_connect( KERNEL_IDT_IRQ_offset + MODULE_PS2_MOUSE_IRQ_number, MODULE_PS2_MOUSE_IO_APIC_register );

					// set default position of pointer
					kernel -> device_mouse_x = kernel -> framebuffer_width_pixel >> STD_SHIFT_2;
					kernel -> device_mouse_y = kernel -> framebuffer_height_pixel >> STD_SHIFT_2;
				}
			}
		}
	}

	// connect PS2 controller interrupt handler for device: keyboard
	kernel -> idt_mount( KERNEL_IDT_IRQ_offset + MODULE_PS2_KEYBOARD_IRQ_number, KERNEL_IDT_TYPE_irq, (uint64_t) module_ps2_keyboard_entry );

	// connect interrupt vector from IDT table in IOAPIC controller
	kernel -> io_apic_connect( KERNEL_IDT_IRQ_offset + MODULE_PS2_KEYBOARD_IRQ_number, MODULE_PS2_KEYBOARD_IO_APIC_register );
}

void _entry( uintptr_t kernel_ptr ) {
	// preserve kernel structure pointer
	kernel = (struct KERNEL *) kernel_ptr;

	// initialize available PS2 devices
	module_ps2_init();

	// hold the door
	while( TRUE ) kernel -> time_sleep( TRUE );
}
