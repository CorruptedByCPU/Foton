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

struct MODULE_USB_STRUCTURE_UHCI_QUEUE *module_usb_queue_1u	= EMPTY;
struct MODULE_USB_STRUCTURE_UHCI_QUEUE *module_usb_queue_8u	= EMPTY;
struct MODULE_USB_STRUCTURE_UHCI_QUEUE *module_usb_queue_16u	= EMPTY;

uint16_t module_usb_keyboard_matrix_low[] = {
	EMPTY,			// 0x00
	EMPTY,			// 0x01
	EMPTY,			// 0x02
	EMPTY,			// 0x03
	'a',			// 0x04
	'b',			// 0x05
	'c',			// 0x06
	'd',			// 0x07
	'e',			// 0x08
	'f',			// 0x09
	'g',			// 0x0A
	'h',			// 0x0B
	'i',			// 0x0C
	'j',			// 0x0D
	'k',			// 0x0E
	'l',			// 0x0F
	'm',			// 0x10
	'n',			// 0x11
	'o',			// 0x12
	'p',			// 0x13
	'q',			// 0x14
	'r',			// 0x15
	's',			// 0x16
	't',			// 0x17
	'u',			// 0x18
	'v',			// 0x19
	'w',			// 0x1A
	'x',			// 0x1B
	'y',			// 0x1C
	'z',			// 0x1D
	'1',			// 0x1E
	'2',			// 0x1F
	'3',			// 0x20
	'4',			// 0x21
	'5',			// 0x22
	'6',			// 0x23
	'7',			// 0x24
	'8',			// 0x25
	'9',			// 0x26
	'0',			// 0x27
	STD_KEY_ENTER,		// 0x28
	STD_KEY_ESC,		// 0x29
	STD_KEY_BACKSPACE,	// 0x2A
	STD_KEY_TAB,		// 0x2B
	STD_KEY_SPACE,		// 0x2C
	'-',			// 0x2D
	'=',			// 0x2E
	'[',			// 0x2F
	']',			// 0x30
	'\\',			// 0x31
	EMPTY,			// 0x32
	';',			// 0x33
	'\'',			// 0x34
	'`',			// 0x35
	',',			// 0x36
	'.',			// 0x37
	'/',			// 0x38
	STD_KEY_CAPSLOCK,	// 0x39
	STD_KEY_F1,		// 0x3A
	STD_KEY_F2,		// 0x3B
	STD_KEY_F3,		// 0x3C
	STD_KEY_F4,		// 0x3D
	STD_KEY_F5,		// 0x3E
	STD_KEY_F6,		// 0x3F
	STD_KEY_F7,		// 0x40
	STD_KEY_F8,		// 0x41
	STD_KEY_F9,		// 0x42
	STD_KEY_F10,		// 0x43
	STD_KEY_F11,		// 0x44
	STD_KEY_F12,		// 0x45
	EMPTY,			// 0x46
	EMPTY,			// 0x47
	EMPTY,			// 0x48
	STD_KEY_INSERT,		// 0x49
	STD_KEY_HOME,		// 0x4A
	STD_KEY_PAGE_UP,	// 0x4B
	STD_KEY_DELETE,		// 0x4C
	STD_KEY_END,		// 0x4D
	STD_KEY_PAGE_DOWN,	// 0x4E
	STD_KEY_ARROW_RIGHT,	// 0x4F
	STD_KEY_ARROW_LEFT,	// 0x50
	STD_KEY_ARROW_DOWN,	// 0x51
	STD_KEY_ARROW_UP	// 0x52
};

uint16_t module_usb_keyboard_matrix_high[] = {
	EMPTY,			// 0x00
	EMPTY,			// 0x01
	EMPTY,			// 0x02
	EMPTY,			// 0x03
	'A',			// 0x04
	'B',			// 0x05
	'C',			// 0x06
	'D',			// 0x07
	'E',			// 0x08
	'F',			// 0x09
	'G',			// 0x0A
	'H',			// 0x0B
	'I',			// 0x0C
	'J',			// 0x0D
	'K',			// 0x0E
	'L',			// 0x0F
	'M',			// 0x10
	'N',			// 0x11
	'O',			// 0x12
	'P',			// 0x13
	'Q',			// 0x14
	'R',			// 0x15
	'S',			// 0x16
	'T',			// 0x17
	'U',			// 0x18
	'V',			// 0x19
	'W',			// 0x1A
	'X',			// 0x1B
	'Y',			// 0x1C
	'Z',			// 0x1D
	'!',			// 0x1E
	'@',			// 0x1F
	'#',			// 0x20
	'$',			// 0x21
	'%',			// 0x22
	'^',			// 0x23
	'&',			// 0x24
	'*',			// 0x25
	'(',			// 0x26
	')',			// 0x27
	STD_KEY_ENTER,		// 0x28
	STD_KEY_ESC,		// 0x29
	STD_KEY_BACKSPACE,	// 0x2A
	STD_KEY_TAB,		// 0x2B
	STD_KEY_SPACE,		// 0x2C
	'_',			// 0x2D
	'+',			// 0x2E
	'{',			// 0x2F
	'}',			// 0x30
	'|',			// 0x31
	EMPTY,			// 0x32
	':',			// 0x33
	'"',			// 0x34
	'~',			// 0x35
	'<',			// 0x36
	'>',			// 0x37
	'?',			// 0x38
	STD_KEY_CAPSLOCK,	// 0x39
	STD_KEY_F1,		// 0x3A
	STD_KEY_F2,		// 0x3B
	STD_KEY_F3,		// 0x3C
	STD_KEY_F4,		// 0x3D
	STD_KEY_F5,		// 0x3E
	STD_KEY_F6,		// 0x3F
	STD_KEY_F7,		// 0x40
	STD_KEY_F8,		// 0x41
	STD_KEY_F9,		// 0x42
	STD_KEY_F10,		// 0x43
	STD_KEY_F11,		// 0x44
	STD_KEY_F12,		// 0x45
	EMPTY,
	EMPTY,
	EMPTY,
	STD_KEY_INSERT,		// 0x49
	STD_KEY_HOME,		// 0x4A
	STD_KEY_PAGE_UP,	// 0x4B
	STD_KEY_DELETE,		// 0x4C
	STD_KEY_END,		// 0x4D
	STD_KEY_PAGE_DOWN,	// 0x4E
	STD_KEY_ARROW_RIGHT,	// 0x4F
	STD_KEY_ARROW_LEFT,	// 0x50
	STD_KEY_ARROW_DOWN,	// 0x51
	STD_KEY_ARROW_UP	// 0x52
};
