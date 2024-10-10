/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	MODULE_USB
	#define	MODULE_USB

	#define	MODULE_USB_DEVICE_TYPE_HID_KEYBOARD	0x01
	#define	MODULE_USB_DEVICE_TYPE_HID_MOUSE	0x02
	#define	MODULE_USB_DEVICE_TYPE_HID_STORAGE	0x03

	#define	MODULE_USB_CONTROLLER_limit					8
	#define	MODULE_USB_PORT_limit						16

	enum MODULE_USB_CONTROLLER_TYPE {
		MODULE_USB_CONTROLLER_TYPE_UNKNOWN,
		MODULE_USB_CONTROLLER_TYPE_UHCI,
		MODULE_USB_CONTROLLER_TYPE_OHCI,
		MODULE_USB_CONTROLLER_TYPE_EHCI,
		MODULE_USB_CONTROLLER_TYPE_xHCI
	};

	struct MODULE_USB_STRUCTURE_CONTROLLER {
		uint8_t		type;

		uintptr_t	base_address;
		uint8_t		limit;
		uint8_t		mmio_semaphore;

		uint64_t	frame_base_address;
	};

	struct MODULE_USB_STRUCTURE_PORT {
		uint8_t		status;
		uint8_t		type;

		uint8_t		id_controller;
		uint8_t		id_port;
		uint8_t		id_address;
		uint8_t		id_endpoint;

		uint8_t		low_speed		: 1;
		uint8_t		max_packet_length;
		uint8_t		toggle;
	};

	#define	MODULE_USB_PACKET_TYPE_direction_device_to_host			0b10000000
	#define	MODULE_USB_PACKET_TYPE_direction_host_to_device			0b00000000
	#define	MODULE_USB_PACKET_TYPE_subtype_standard				0b00000000
	#define	MODULE_USB_PACKET_TYPE_subtype_vendor				0b01000000
	#define	MODULE_USB_PACKET_TYPE_subtype_class				0b00100000
	#define	MODULE_USB_PACKET_TYPE_recipient_device				0b00000000
	#define	MODULE_USB_PACKET_TYPE_recipient_interface			0b00000001
	#define	MODULE_USB_PACKET_TYPE_recipient_endpoint			0b00000010
	#define	MODULE_USB_PACKET_TYPE_recipient_other				0b00000011

	#define	MODULE_USB_PACKET_REQUEST_status_get				0x00
	#define	MODULE_USB_PACKET_REQUEST_feature_clear				0x01
	#define	MODULE_USB_PACKET_REQUEST_feature_set				0x03
	#define	MODULE_USB_PACKET_REQUEST_address_set				0x05
	#define	MODULE_USB_PACKET_REQUEST_descriptor_get			0x06
	#define	MODULE_USB_PACKET_REQUEST_descriptor_set			0x07
	#define	MODULE_USB_PACKET_REQUEST_configuration_get			0x08
	#define	MODULE_USB_PACKET_REQUEST_configuration_set			0x09
	#define	MODULE_USB_PACKET_REQUEST_idle_set				0x0A
	#define	MODULE_USB_PACKET_REQUEST_interface_set				0x0B
	// #define	MODULE_USB_PACKET_REQUEST_interface_set				0x11
	// #define	MODULE_USB_PACKET_REQUEST_sync_frame				0x12

	#define	MODULE_USB_PACKET_VALUE_descriptor_type_device			0x0100
	#define	MODULE_USB_PACKET_VALUE_descriptor_type_configuration		0x0200
	#define	MODULE_USB_PACKET_VALUE_descriptor_type_string			0x0300
	#define	MODULE_USB_PACKET_VALUE_descriptor_type_interface		0x0400
	#define	MODULE_USB_PACKET_VALUE_descriptor_type_endpoint		0x0500
	#define	MODULE_USB_PACKET_VALUE_descriptor_type_device_qualifier	0x0600
	#define	MODULE_USB_PACKET_VALUE_descriptor_type_other_speed_config	0x0700
	#define	MODULE_USB_PACKET_VALUE_descriptor_type_interface_power		0x0800
	#define	MODULE_USB_PACKET_VALUE_descriptor_type_report			0x2200

	#define MODULE_USB_HID_KEYBOARD_KEY_CODE_CTRL_LEFT			0b00000001
	#define	MODULE_USB_HID_KEYBOARD_KEY_CODE_SHIFT_LEFT			0b00000010
	#define	MODULE_USB_HID_KEYBOARD_KEY_CODE_ALT_LEFT			0b00000100
	#define	MODULE_USB_HID_KEYBOARD_KEY_CODE_MENU_LEFT			0b00001000
	#define MODULE_USB_HID_KEYBOARD_KEY_CODE_CTRL_RIGHT			0b00010000
	#define	MODULE_USB_HID_KEYBOARD_KEY_CODE_SHIFT_RIGHT			0b00100000
	#define	MODULE_USB_HID_KEYBOARD_KEY_CODE_ALT_RIGHT			0b01000000
#endif