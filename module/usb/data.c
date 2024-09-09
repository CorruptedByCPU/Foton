/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct KERNEL *kernel = EMPTY;

uint8_t module_usb_string_port[] = "port";
uint8_t module_usb_string_memory[] = "memory";

struct MODULE_USB_STRUCTURE_CONTROLLER *module_usb_controller = EMPTY;
struct MODULE_USB_STRUCTURE_PORT *module_usb_port = EMPTY;

uint8_t module_usb_controller_count = EMPTY;
uint8_t	module_usb_port_count = 0;	// 0th entry is reserved, others mean as device assigned enumeration

struct MODULE_USB_STRUCTURE_QUEUE *module_usb_queue_1ms = EMPTY;
struct MODULE_USB_STRUCTURE_QUEUE *module_usb_queue_8ms = EMPTY;

uint16_t module_usb_keyboard_matrix_low[] = {
	EMPTY,
	EMPTY,
	EMPTY,
	EMPTY,
	'a',	// 0x04
	'b',	// 0x05
	'c',	// 0x06
	'd',	// 0x07
	'e',	// 0x08
	'f',	// 0x09
	'g',	// 0x0A
	'h',	// 0x0B
	'i',	// 0x0C
	'j',	// 0x0D
	'k',	// 0x0E
	'l',	// 0x0F
	'm',	// 0x10
	'n',	// 0x11
	'o',	// 0x12
	'p',	// 0x13
	'q',	// 0x14
	'r',	// 0x15
	's',	// 0x16
	't',	// 0x17
	'u',	// 0x18
	'v',	// 0x19
	'w',	// 0x1A
	'x',	// 0x1B
	'y',	// 0x1C
	'z',	// 0x1D
	'1',	// 0x1E
	'2',	// 0x1F
	'3',	// 0x20
	'4',	// 0x21
	'5',	// 0x22
	'6',	// 0x23
	'7',	// 0x24
	'8',	// 0x25
	'9',	// 0x26
	'0',	// 0x27
};
