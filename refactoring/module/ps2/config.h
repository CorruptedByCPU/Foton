/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	MODULE_PS2
	#define	MODULE_PS2

	#define	MODULE_PS2_KEYBOARD_IRQ_number			0x01
	#define	MODULE_PS2_KEYBOARD_IO_APIC_register		KERNEL_IO_APIC_iowin + (MODULE_PS2_KEYBOARD_IRQ_number * 0x02)

	#define	MODULE_PS2_MOUSE_IRQ_number			0x0C
	#define	MODULE_PS2_MOUSE_IO_APIC_register		KERNEL_IO_APIC_iowin + (MODULE_PS2_MOUSE_IRQ_number * 0x02)

	#define	MODULE_PS2_PORT_DATA				0x0060
	#define	MODULE_PS2_PORT_COMMAND_OR_STATUS		0x0064

	#define	MODULE_PS2_COMMAND_CONFIGURATION_GET		0x20
	#define	MODULE_PS2_COMMAND_CONFIGURATION_SET		0x60
	#define	MODULE_PS2_COMMAND_PORT_SECOND			0xD4

	#define	MODULE_PS2_STATUS_output			0b00000001
	#define	MODULE_PS2_STATUS_input				0b00000010
	#define	MODULE_PS2_STATUS_output_second			0b00100000

	#define	MODULE_PS2_CONFIGURATION_PORT_SECOND_INTERRUPT	0b00000010
	#define	MODULE_PS2_CONFIGURATION_PORT_SECOND_CLOCK	0b00100000

	#define	MODULE_PS2_DEVICE_ID_GET			0xF2
	#define	MODULE_PS2_DEVICE_SAMPLE_RATE_SET		0xF3
	#define	MODULE_PS2_DEVICE_PACKETS_ENABLE		0xF4
	#define	MODULE_PS2_DEVICE_SET_DEFAULT			0xF6
	#define	MODULE_PS2_DEVICE_RESET				0xFF

	#define	MODULE_PS2_ANSWER_ACKNOWLEDGED			0xFA
	#define	MODULE_PS2_ANSWER_SELF_TEST_SUCCESS		0xAA

	#define	MODULE_PS2_KEYBOARD_sequence			0xE0
	#define	MODULE_PS2_KEYBOARD_sequence_alternative	0xE1

	#define	MODULE_PS2_MOUSE_PACKET_LMB			0b00000001
	#define	MODULE_PS2_MOUSE_PACKET_RMB			0b00000010
	#define	MODULE_PS2_MOUSE_PACKET_MMB			0b00000100
	#define	MODULE_PS2_MOUSE_PACKET_ALWAYS_ONE		0b00001000
	#define	MODULE_PS2_MOUSE_PACKET_X_SIGNED		0b00010000
	#define	MODULE_PS2_MOUSE_PACKET_Y_SIGNED		0b00100000
	#define	MODULE_PS2_MOUSE_PACKET_OVERFLOW_x		0b01000000
	#define	MODULE_PS2_MOUSE_PACKET_OVERFLOW_y		0b10000000

	#define	MODULE_PS2_CACHE_limit				7

	// external routines (assembly language)
	extern void module_ps2_mouse_entry( void );
	extern void module_ps2_keyboard_entry( void );
#endif