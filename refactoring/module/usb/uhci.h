/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	MODULE_USB_UHCI
	#define	MODULE_USB_UHCI

	#define	MODULE_USB_UHCI_PORT_STATUS_AND_CONTROL_current_connect_status		0x0001
	#define	MODULE_USB_UHCI_PORT_STATUS_AND_CONTROL_connect_status_change		0x0002
	#define	MODULE_USB_UHCI_PORT_STATUS_AND_CONTROL_port_enabled			0x0004
	#define	MODULE_USB_UHCI_PORT_STATUS_AND_CONTROL_port_enable_change		0x0008
	#define	MODULE_USB_UHCI_PORT_STATUS_AND_CONTROL_line_status			0x0010
	#define	MODULE_USB_UHCI_PORT_STATUS_AND_CONTROL_resume_detect			0x0020
	#define	MODULE_USB_UHCI_PORT_STATUS_AND_CONTROL_reserved_should_be_one		0x0040
	#define	MODULE_USB_UHCI_PORT_STATUS_AND_CONTROL_high_speed_device_attached	0x0080
	#define	MODULE_USB_UHCI_PORT_STATUS_AND_CONTROL_port_reset			0x0100
	#define	MODULE_USB_UHCI_PORT_STATUS_AND_CONTROL_reserved			0x0C00
	#define	MODULE_USB_UHCI_PORT_STATUS_AND_CONTROL_suspend				0x1000

	struct MODULE_USB_STRUCTURE_UHCI_REGISTER {
		uint16_t	command;
		uint16_t	status;
		uint16_t	interrupt_enable;
		uint16_t	frame_number;
		uint32_t	frame_list_base_address;
		uint8_t		start_of_frame_modify;
		uint8_t		reserved[ 3 ];
		uint16_t	port[ TRUE ];	// by default 2, but correct amount is determined by MODULE_USB_STRUCTURE_CONTROLLER.limit
	} __attribute__( (packed) );

	struct	MODULE_USB_STRUCTURE_UHCI_PACKET {
		uint8_t			type;
		uint8_t			request;
		uint16_t		value;
		uint16_t		index;
		uint16_t		length;
	} __attribute__( (packed) );

	#define	MODULE_USB_UHCI_TD_STATUS_active		0x80

	#define	MODULE_USB_UHCI_TD_PACKET_IDENTIFICATION_setup	0x2D
	#define	MODULE_USB_UHCI_TD_PACKET_IDENTIFICATION_in	0x69
	#define	MODULE_USB_UHCI_TD_PACKET_IDENTIFICATION_out	0xE1

	struct MODULE_USB_STRUCTURE_UHCI_TD {
		uint8_t			flags 			: 4;
		uint32_t		link_pointer		: 28;
		uint16_t		actual_length		: 11;
		uint8_t			reserved0		: 5;
		volatile uint8_t	status			: 8;
		uint8_t			ioc			: 1;
		uint8_t			iso			: 1;
		uint8_t			low_speed		: 1;
		uint8_t			error_counter		: 2;
		uint8_t			short_packet		: 1;
		uint8_t			reserved1		: 2;
		uint8_t			packet_identification	: 8;
		uint8_t			device_address		: 7;
		uint8_t			endpoint		: 4;
		uint8_t			data_toggle		: 1;
		uint8_t			reserved2		: 1;
		uint16_t		max_length		: 11;
		uint32_t		buffer_pointer;
		uint32_t		reserved[ 4 ];
	} __attribute__( (packed) );

	#define	MODULE_USB_UHCI_QTD_FLAG_mask				0x0F
	#define	MODULE_USB_UHCI_QTD_FLAG_terminate			0x01
	#define	MODULE_USB_UHCI_QTD_FLAG_queue				0x02
	#define	MODULE_USB_UHCI_QTD_FLAG_depth_first			0x04

	struct MODULE_USB_STRUCTURE_UHCI_QUEUE {
		uint32_t		head_link_pointer_and_flags;
		uint32_t		element_link_pointer_and_flags;
		uint32_t		preserved_head;
		uint32_t		preserved_element;
	} __attribute__( (packed) );

	struct MODULE_USB_STRUCTURE_UHCI_DESCRIPTOR_DEFAULT {
		uint8_t			length;
		uint8_t			type;
	} __attribute__( (packed) );

	struct MODULE_USB_STRUCTURE_UHCI_DESCRIPTOR_DEVICE {
		uint8_t			length;
		uint8_t			type;
		uint16_t		release_number;
		uint8_t			class;
		uint8_t			subclass;
		uint8_t			protocol;
		uint8_t			max_packet_size;
		uint16_t		vendor_id;
		uint16_t		product_id;
		uint16_t		device_release_number;
		uint8_t			offset_manufacturer;
		uint8_t			offset_product;
		uint8_t			offset_serial_number;
		uint8_t			configurations;
	} __attribute__( (packed) );

	struct MODULE_USB_STRUCTURE_UHCI_DESCRIPTOR_CONFIGURATION {
		uint8_t			length;
		uint8_t			type;
		uint16_t		total_length;
		uint8_t			interface_count;
		uint8_t			config_value;
		uint8_t			config_index;
		uint8_t			attributes;
		uint8_t			max_power;
	} __attribute__( (packed) );

	struct MODULE_USB_STRUCTURE_UHCI_DESCRIPTOR_INTERFACE {
		uint8_t			length;
		uint8_t			type;
		uint8_t			interface_id;
		uint8_t			alternate_settings;
		uint8_t			endpoint_count;
		uint8_t			class;
		uint8_t			subclass;
		uint8_t			protocol;
		uint8_t			interface_index;
	} __attribute__( (packed) );

	struct MODULE_USB_STRUCTURE_UHCI_DESCRIPTOR_ENDPOINT {
		uint8_t			length;
		uint8_t			type;
		uint8_t			address;
		uint8_t			attributes;
		uint16_t		max_packet_size;
		uint8_t			interval;
	} __attribute__( (packed) );

	void module_usb_uhci_init( uint8_t c );
	void module_usb_uhci_queue( uint32_t *frame_list );
	uintptr_t module_usb_uhci_queue_empty( void );
	uint64_t module_usb_uhci_queue_insert( uint8_t unit, uint8_t type, uintptr_t source );
	void module_usb_uhci_descriptor( struct MODULE_USB_STRUCTURE_PORT *p, uint8_t length, uintptr_t target, uint8_t flow, uintptr_t packet );
	uint8_t module_usb_uhci_descriptor_io( struct MODULE_USB_STRUCTURE_PORT *port, uint8_t length, uintptr_t target, uint8_t flow, uint8_t queue );
	void module_usb_uhci_queue_remove( uint8_t unit, uint64_t entry );
	uint16_t module_usb_uhci_device_init( uint8_t c, uint8_t p );
	uint8_t module_usb_uhci_device_setup( struct MODULE_USB_STRUCTURE_PORT *port );
#endif
