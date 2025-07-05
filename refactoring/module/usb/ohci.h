/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	MODULE_USB_OHCI
	#define	MODULE_USB_OHCI

	#define	MODULE_USB_OHCI_REGISTER_COMMAND_STATUS_host_controller_reset		(1 << 0)

	#define	MODULE_USB_OHCI_REGISTER_CONTROL_host_controller_functional_state	(0b11 << 6)

	#define	MODULE_USB_OHCI_REGISTER_FM_INTERVAL_update				(1 << 31)
	#define	MODULE_USB_OHCI_REGISTER_FM_INTERVAL_default				0x2EDF
	#define	MODULE_USB_OHCI_REGISTER_FM_INTERVAL_full_speed				0x2A2F0000

	#define	MODULE_USB_OHCI_REGISTER_PERIODIC_START_default				0x2A2F

	#define	MODULE_USB_OHCI_REGISTER_RH_DESCRIPTOR_A_power_individual		(1 << 8)
	#define	MODULE_USB_OHCI_REGISTER_RH_DESCRIPTOR_A_always_on			(1 << 9)

	struct MODULE_USB_STRUCTURE_OHCI_REGISTER {
		uint32_t	revision;
		uint32_t	control;
		uint32_t	command_status;
		uint32_t	interrupt_status;
		uint32_t	interrupt_enable;
		uint32_t	interrupt_disable;
		uint32_t	hcca;
		uint32_t	period_counter;
		uint32_t	control_head;
		uint32_t	control_current;
		uint32_t	bulk_head;
		uint32_t	bulk_current;
		uint32_t	done_head;
		uint32_t	fm_interval;
		uint32_t	fm_remaining;
		uint32_t	fm_number;
		uint32_t	periodic_start;
		uint32_t	ls_treshold;
		uint32_t	rh_descriptor_a;
		uint32_t	rh_descriptor_b;
		uint32_t	rh_status;
		uint32_t	rh_port_status[ TRUE ];
	} __attribute__( (packed) );

	void module_usb_ohci_init( uint8_t c );
#endif