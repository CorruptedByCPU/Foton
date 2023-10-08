/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	DRIVER_PS2
	#define	DRIVER_PS2

	#define	DRIVER_PS2_KEYBOARD_IRQ_number			0x01
	#define	DRIVER_PS2_KEYBOARD_IO_APIC_register		KERNEL_IO_APIC_iowin + (DRIVER_PS2_KEYBOARD_IRQ_number * 0x02)

	#define	DRIVER_PS2_MOUSE_IRQ_number			0x0C
	#define	DRIVER_PS2_MOUSE_IO_APIC_register		KERNEL_IO_APIC_iowin + (DRIVER_PS2_MOUSE_IRQ_number * 0x02)

	#define	DRIVER_PS2_PORT_DATA				0x0060
	#define	DRIVER_PS2_PORT_COMMAND_OR_STATUS		0x0064

	#define	DRIVER_PS2_COMMAND_CONFIGURATION_GET		0x20
	#define	DRIVER_PS2_COMMAND_CONFIGURATION_SET		0x60
	#define	DRIVER_PS2_COMMAND_PORT_SECOND			0xD4

	#define	DRIVER_PS2_STATUS_output			0b00000001
	#define	DRIVER_PS2_STATUS_input				0b00000010
	#define	DRIVER_PS2_STATUS_output_second			0b00100000

	#define	DRIVER_PS2_CONFIGURATION_PORT_SECOND_INTERRUPT	0b00000010
	#define	DRIVER_PS2_CONFIGURATION_PORT_SECOND_CLOCK	0b00100000

	#define	DRIVER_PS2_DEVICE_PACKETS_ENABLE		0xF4
	#define	DRIVER_PS2_DEVICE_SET_DEFAULT			0xF6
	#define	DRIVER_PS2_DEVICE_RESET				0xFF

	#define	DRIVER_PS2_ANSWER_ACKNOWLEDGED			0xFA
	#define	DRIVER_PS2_ANSWER_SELF_TEST_SUCCESS		0xAA

	#define	DRIVER_PS2_KEYBOARD_sequence			0xE0
	#define	DRIVER_PS2_KEYBOARD_sequence_alternative	0xE1

	// external routines (assembly language)
	extern void driver_ps2_mouse_entry( void );
	extern void driver_ps2_keyboard_entry( void );
#endif