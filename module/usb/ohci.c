/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void module_usb_ohci_init( uint8_t c ) {
	// RESET ---------------------------------------------------------------

	// properties of OHCI controller
	volatile struct MODULE_USB_STRUCTURE_OHCI_REGISTER *ohci = (struct MODULE_USB_STRUCTURE_OHCI_REGISTER *) module_usb_controller[ c ].base_address;

	// OHCI version 1.0?
	if( (ohci -> revision & STD_MASK_byte) != 0x10 ) return;	// no

	// reset controller
	ohci -> command_status = MODULE_USB_OHCI_REGISTER_COMMAND_STATUS_host_controller_reset;

	// do not wait more than 1ms for reset
	uint64_t wait = kernel -> time_rtc + 1;
	while( ohci -> command_status & MODULE_USB_OHCI_REGISTER_COMMAND_STATUS_host_controller_reset ) if( wait < kernel -> time_rtc ) return;	// timeout

	// controller is in functional state?
	if( (ohci -> control & MODULE_USB_OHCI_REGISTER_CONTROL_host_controller_functional_state) != MODULE_USB_OHCI_REGISTER_CONTROL_host_controller_functional_state ) return;	// no

	// preserve fm_interval
	uint32_t fm_interval = ohci -> fm_interval;

	// correct interval?
	if( fm_interval != 0x2EDF ) return;	// no

	// SETUP ---------------------------------------------------------------

	// acquire size of HCCA
	ohci -> hcca = STD_MAX_unsigned;
	volatile uint32_t bytes = ~ohci -> hcca + 1;

	// reserve memory area below 1 MiB
	uintptr_t base_address = kernel -> memory_alloc_low( MACRO_PAGE_ALIGN_UP( bytes ) >> STD_SHIFT_PAGE );

	// debug
	kernel -> log( (uint8_t *) "[USB].%u PCI %2X:%2X.%u - HCCA memory block at 0x%X\n", c, module_usb_controller[ c ].pci.bus, module_usb_controller[ c ].pci.device, module_usb_controller[ c ].pci.function, base_address );

	// reset all ports
	ohci -> control = EMPTY;
	// wait at least 50 ms
	kernel -> time_sleep( 64 );
	// after 64ms, stop reset
	ohci -> control = MODULE_USB_OHCI_REGISTER_CONTROL_host_controller_functional_state;
}