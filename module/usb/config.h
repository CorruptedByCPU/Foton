/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	DRIVER_USB
	#define	DRIVER_USB

	#define	DRIVER_USB_BASE_ADDRESS_type	0b0001

	#define	DRIVER_USB_CONTROLLER_limit	1
	#define	DRIVER_USB_PORT_limit		2

	#define	DRIVER_USB_CONTROLLER_COMMAND_run_stop				0b00000001
	#define	DRIVER_USB_CONTROLLER_COMMAND_host_controller_reset		0b00000010
	#define	DRIVER_USB_CONTROLLER_COMMAND_global_reset			0b00000100
	#define	DRIVER_USB_CONTROLLER_COMMAND_enter_global_suspended_mode	0b00001000
	#define	DRIVER_USB_CONTROLLER_COMMAND_force_global_resume		0b00010000
	#define	DRIVER_USB_CONTROLLER_COMMAND_software_debug			0b00100000
	#define	DRIVER_USB_CONTROLLER_COMMAND_configure_flag			0b01000000
	#define	DRIVER_USB_CONTROLLER_COMMAND_max_packet			0b10000000

	#define	DRIVER_USB_CONTROLLER_STATUS_usb_interrupt			0b00000001
	#define	DRIVER_USB_CONTROLLER_STATUS_usb_error_interrupt		0b00000010
	#define	DRIVER_USB_CONTROLLER_STATUS_resume_detect			0b00000100
	#define	DRIVER_USB_CONTROLLER_STATUS_host_system_error			0b00001000
	#define	DRIVER_USB_CONTROLLER_STATUS_host_controller_process_error	0b00010000
	#define	DRIVER_USB_CONTROLLER_STATUS_host_controller_halted		0b00100000

	#define	DRIVER_USB_CONTROLLER_INTERRUPT_ENABLE_timeout_crc		0b00000001
	#define	DRIVER_USB_CONTROLLER_INTERRUPT_ENABLE_resume			0b00000010
	#define	DRIVER_USB_CONTROLLER_INTERRUPT_ENABLE_on_complete		0b00000100
	#define	DRIVER_USB_CONTROLLER_INTERRUPT_ENABLE_short_packet		0b00001000

	#define	DRIVER_USB_PORT_STATUS_AND_CONTROL_current_connect_status	0b0000000000000001
	#define	DRIVER_USB_PORT_STATUS_AND_CONTROL_connect_status_change	0b0000000000000010
	#define	DRIVER_USB_PORT_STATUS_AND_CONTROL_port_enabled			0b0000000000000100
	#define	DRIVER_USB_PORT_STATUS_AND_CONTROL_port_enable_change		0b0000000000001000
	#define	DRIVER_USB_PORT_STATUS_AND_CONTROL_line_status			0b0000000000110000
	#define	DRIVER_USB_PORT_STATUS_AND_CONTROL_resume_detect		0b0000000001000000
	#define	DRIVER_USB_PORT_STATUS_AND_CONTROL_reserved_should_be_one	0b0000000010000000
	#define	DRIVER_USB_PORT_STATUS_AND_CONTROL_low_speed_device_attached	0b0000000100000000
	#define	DRIVER_USB_PORT_STATUS_AND_CONTROL_port_reset			0b0000001000000000
	#define	DRIVER_USB_PORT_STATUS_AND_CONTROL_reserved			0b0000110000000000
	#define	DRIVER_USB_PORT_STATUS_AND_CONTROL_suspend			0b0001000000000000

	#define	DRIVER_USB_PORT_FLAG_reserved			0b00000001

	#define	DRIVER_USB_FRAME_FLAG_terminate			0b0001
	#define	DRIVER_USB_FRAME_FLAG_queue			0b0010

	struct DRIVER_USB_CONTROLLER_STRUCTURE {
		uint8_t		type;
		uintptr_t	base_address;
		uint16_t	size_byte;
		uint64_t	frame_base_address;
		uint8_t		irq_line;
	};

	struct DRIVER_USB_PORT_STRUCTURE {
		uint8_t		flags;
		uint8_t		controller_id;
		uint8_t		port_id;
		uint8_t		address_id;
	};

	struct DRIVER_USB_REGISTER_STRUCTURE {
		uint16_t	command;
		uint16_t	status;
		uint16_t	interrupt_enable;
		uint16_t	frame_number;
		uint32_t	frame_list_base_address;
		uint8_t		start_of_frame_modify;
		uint8_t		reserved[ 3 ];
		uint16_t	port[ 2 ];	// by default 2, but correct amount is determined by DRIVER_USB_CONTROLLER_STRUCTURE.size_byte
	} __attribute__( (packed) );

	struct DRIVER_USB_QUEUE_STRUCTURE {
		uint32_t	head_link_pointer_and_flags;
		uint32_t	element_link_pointer_and_flags;
		uint32_t	reserved[ 2 ];
	} __attribute__( (packed) );

	struct DRIVER_USB_TRANSFER_DESCRIPTOR_STRUCTURE {
		uint32_t	link_pointer_and_flags;
		uint32_t	descriptor_controls;
		uint32_t	transfer_controls;
		uint32_t	buffer_pointer;
		uint32_t	reserved[ 4 ];
	} __attribute__( (packed) );
#endif