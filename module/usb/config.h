/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	MODULE_USB
	#define	MODULE_USB

	#define	MODULE_USB_BASE_ADDRESS_type					0b0001

	#define	MODULE_USB_CONTROLLER_limit					1
	#define	MODULE_USB_PORT_limit						2	// less than 128

	#define	MODULE_USB_CONTROLLER_COMMAND_run_stop				0b00000001
	#define	MODULE_USB_CONTROLLER_COMMAND_host_controller_reset		0b00000010
	#define	MODULE_USB_CONTROLLER_COMMAND_global_reset			0b00000100
	#define	MODULE_USB_CONTROLLER_COMMAND_enter_global_suspended_mode	0b00001000
	#define	MODULE_USB_CONTROLLER_COMMAND_force_global_resume		0b00010000
	#define	MODULE_USB_CONTROLLER_COMMAND_software_debug			0b00100000
	#define	MODULE_USB_CONTROLLER_COMMAND_configure_flag			0b01000000
	#define	MODULE_USB_CONTROLLER_COMMAND_max_packet			0b10000000

	#define	MODULE_USB_CONTROLLER_STATUS_usb_interrupt			0b00000001
	#define	MODULE_USB_CONTROLLER_STATUS_usb_error_interrupt		0b00000010
	#define	MODULE_USB_CONTROLLER_STATUS_resume_detect			0b00000100
	#define	MODULE_USB_CONTROLLER_STATUS_host_system_error			0b00001000
	#define	MODULE_USB_CONTROLLER_STATUS_host_controller_process_error	0b00010000
	#define	MODULE_USB_CONTROLLER_STATUS_host_controller_halted		0b00100000

	#define	MODULE_USB_CONTROLLER_INTERRUPT_ENABLE_timeout_crc		0b00000001
	#define	MODULE_USB_CONTROLLER_INTERRUPT_ENABLE_resume			0b00000010
	#define	MODULE_USB_CONTROLLER_INTERRUPT_ENABLE_on_complete		0b00000100
	#define	MODULE_USB_CONTROLLER_INTERRUPT_ENABLE_short_packet		0b00001000

	#define	MODULE_USB_PORT_STATUS_AND_CONTROL_current_connect_status	0b0000000000000001
	#define	MODULE_USB_PORT_STATUS_AND_CONTROL_connect_status_change	0b0000000000000010
	#define	MODULE_USB_PORT_STATUS_AND_CONTROL_port_enabled			0b0000000000000100
	#define	MODULE_USB_PORT_STATUS_AND_CONTROL_port_enable_change		0b0000000000001000
	#define	MODULE_USB_PORT_STATUS_AND_CONTROL_line_status			0b0000000000110000
	#define	MODULE_USB_PORT_STATUS_AND_CONTROL_resume_detect		0b0000000001000000
	#define	MODULE_USB_PORT_STATUS_AND_CONTROL_reserved_should_be_one	0b0000000010000000
	#define	MODULE_USB_PORT_STATUS_AND_CONTROL_high_speed_device_attached	0b0000000100000000
	#define	MODULE_USB_PORT_STATUS_AND_CONTROL_port_reset			0b0000001000000000
	#define	MODULE_USB_PORT_STATUS_AND_CONTROL_reserved			0b0000110000000000
	#define	MODULE_USB_PORT_STATUS_AND_CONTROL_suspend			0b0001000000000000

	#define	MODULE_USB_PORT_FLAG_reserved					0b00000001

	#define	MODULE_USB_DEFAULT_FLAG_mask					0x0F
	#define	MODULE_USB_DEFAULT_FLAG_terminate				0b0001
	#define	MODULE_USB_DEFAULT_FLAG_queue					0b0010
	#define	MODULE_USB_DEFAULT_FLAG_data					0b0100

	#define	MODULE_USB_TRANSFER_DESCRIPTOR_DEVICE_CONTROL_PID_setup			0
	#define	MODULE_USB_TRANSFER_DESCRIPTOR_DESCRIPTOR_CONTROL_high_speed_device	26
	#define	MODULE_USB_TRANSFER_DESCRIPTOR_DESCRIPTOR_CONTROL_error_counter		27

	#define	MODULE_USB_TRANSFER_DESCRIPTOR_STATUS_active			(1 << 7)

	#define	MODULE_USB_REQUEST_SETUP					0
	#define	MODULE_USB_REQUEST_ADDRESS					1
	#define	MODULE_USB_DESCRIPTOR_SETUP					0x0008000001000680

	#define	MODULE_USB_DEVICE_REQUEST_PACKET_TYPE_direction_device_to_host	0b10000000
	#define	MODULE_USB_DEVICE_REQUEST_PACKET_TYPE_direction_host_to_device	0b00000000
	#define	MODULE_USB_DEVICE_REQUEST_PACKET_TYPE_subtype_standard		0b00000000
	#define	MODULE_USB_DEVICE_REQUEST_PACKET_TYPE_subtype_vendor		0b01000000
	#define	MODULE_USB_DEVICE_REQUEST_PACKET_TYPE_subtype_class		0b00100000
	#define	MODULE_USB_DEVICE_REQUEST_PACKET_TYPE_recipient_device		0b00000000
	#define	MODULE_USB_DEVICE_REQUEST_PACKET_TYPE_recipient_interface	0b00000001
	#define	MODULE_USB_DEVICE_REQUEST_PACKET_TYPE_recipient_endpoint	0b00000010
	#define	MODULE_USB_DEVICE_REQUEST_PACKET_TYPE_recipient_other		0b00000011

	#define	MODULE_USB_STANDARD_REQUEST_status_get				0x00
	#define	MODULE_USB_STANDARD_REQUEST_feature_clear			0x01
	#define	MODULE_USB_STANDARD_REQUEST_feature_set				0x03
	#define	MODULE_USB_STANDARD_REQUEST_address_set				0x05
	#define	MODULE_USB_STANDARD_REQUEST_descriptor_get			0x06
	#define	MODULE_USB_STANDARD_REQUEST_descriptor_set			0x07
	#define	MODULE_USB_STANDARD_REQUEST_configuration_get			0x08
	#define	MODULE_USB_STANDARD_REQUEST_interface_get			0x0A
	#define	MODULE_USB_STANDARD_REQUEST_sync_frame				0x0C

	struct	MODULE_USB_DEVICE_REQUEST_PACKET {
		uint8_t		type;
		uint8_t		request;
		uint16_t	value;
		uint16_t	index;
		uint16_t	length;
	} __attribute__( (packed) );

	struct MODULE_USB_CONTROLLER_STRUCTURE {
		uint8_t		type;
		uintptr_t	base_address;
		uint16_t	size_byte;
		uint64_t	frame_base_address;
		uint8_t		irq_line;
	};

	struct MODULE_USB_PORT_STRUCTURE {
		uint8_t		flags;
		uint8_t		controller_id;
		uint8_t		port_id;
		uint8_t		address_id;
		uint8_t		low_speed;
		uint64_t	max_packet_size;
		uint64_t	default_descriptor_length;
	};

	struct MODULE_USB_REGISTER_STRUCTURE {
		uint16_t	command;
		uint16_t	status;
		uint16_t	interrupt_enable;
		uint16_t	frame_number;
		uint32_t	frame_list_base_address;
		uint8_t		start_of_frame_modify;
		uint8_t		reserved[ 3 ];
		uint16_t	port[ 2 ];	// by default 2, but correct amount is determined by MODULE_USB_CONTROLLER_STRUCTURE.size_byte
	} __attribute__( (packed) );

	struct MODULE_USB_QUEUE_STRUCTURE {
		uint32_t	head_link_pointer_and_flags;
		uint32_t	element_link_pointer_and_flags;
		uint32_t	reserved[ 2 ];
	} __attribute__( (packed) );

	struct MODULE_USB_TD_STRUCTURE {
		uint8_t		flags 		: 4;
		uint32_t	link_pointer	: 28;
		uint16_t	actual_length	: 11;
		uint8_t		reserved0	: 5;
		uint8_t		status		: 8;
		uint8_t		ioc		: 1;
		uint8_t		iso		: 1;
		uint8_t		low_speed	: 1;
		uint8_t		error_counter	: 2;
		uint8_t		short_packet	: 1;
		uint8_t		reserved1	: 2;
		uint8_t		pid		: 8;
		uint8_t		device_address	: 7;
		uint8_t		endpoint	: 4;
		uint8_t		data_toggle	: 1;
		uint8_t		reserved2	: 1;
		uint16_t	max_length	: 11;
		uint32_t	buffer_pointer;
		uint32_t	reserved[ 4 ];
	} __attribute__( (packed) );

	struct MODULE_USB_DESCRIPTOR_STANDARD {
		uint8_t		length;
		uint8_t		type;
		uint16_t	release_number;
		uint8_t		class;
		uint8_t		subclass;
		uint8_t		protocol;
		uint8_t		max_packet_size;
		uint16_t	vendor_id;
		uint16_t	product_id;
		uint16_t	device_release_number;
		uint8_t		offset_manufacturer;
		uint8_t		offset_product;
		uint8_t		offset_serial_number;
		uint8_t		configurations;
	} __attribute__( (packed) );
#endif