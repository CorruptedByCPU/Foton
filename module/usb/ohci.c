/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

// STILL IN PROGRESS !

__attribute__(( preserve_most ))
void module_usb_ohci_irq( void ) {
	// MACRO_DEBUF(); MACRO_DEBUF(); MACRO_DEBUF(); MACRO_DEBUF(); MACRO_DEBUF();

	// tell APIC of current logical processor that hardware interrupt was handled, propely
	kernel -> lapic_base_address -> eoi = EMPTY;
}

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
	uintptr_t base_address = kernel -> memory_alloc_low( MACRO_PAGE_ALIGN_UP( bytes ) >> STD_SHIFT_PAGE ) | KERNEL_PAGE_mirror;

	// debug
	kernel -> log( (uint8_t *) "[USB].%u PCI %2X:%2X.%u - HCCA memory block at 0x%X\n", c, module_usb_controller[ c ].pci.bus, module_usb_controller[ c ].pci.device, module_usb_controller[ c ].pci.function, base_address );

	// reset all ports
	ohci -> control = EMPTY;
	// wait at least 50 ms
	kernel -> time_sleep( 64 );
	// after 64ms, stop reset
	ohci -> control = MODULE_USB_OHCI_REGISTER_CONTROL_host_controller_functional_state;

	// set frame interval and speed
	ohci -> fm_interval = MODULE_USB_OHCI_REGISTER_FM_INTERVAL_default | MODULE_USB_OHCI_REGISTER_FM_INTERVAL_full_speed | MODULE_USB_OHCI_REGISTER_FM_INTERVAL_update;

	// interrupt list after 1200 frames
	ohci -> periodic_start = MODULE_USB_OHCI_REGISTER_PERIODIC_START_default;

	uint32_t rh_descriptor_a = ohci -> rh_descriptor_a & STD_MASK_byte;
	kernel -> log( (uint8_t *) "%u ports available.\n", rh_descriptor_a & 0x011111111 );
	ohci -> rh_descriptor_a = (rh_descriptor_a & ~MODULE_USB_OHCI_REGISTER_RH_DESCRIPTOR_A_power_individual) | MODULE_USB_OHCI_REGISTER_RH_DESCRIPTOR_A_always_on;
	uint32_t rh_descriptor_b = EMPTY;
	for( uint8_t i = 0; i < (rh_descriptor_a & STD_MASK_byte); i++ )
		rh_descriptor_b |= 1 << (i + 17);
	ohci -> rh_descriptor_b = rh_descriptor_b;
	ohci -> hcca = base_address & ~KERNEL_PAGE_mirror;
	ohci -> control_head = EMPTY;
	ohci -> bulk_head = EMPTY;
	ohci -> control = 0x000006B0;
	ohci -> rh_status = 0x00008000;
	ohci -> interrupt_status = 0x00000004;
	ohci -> interrupt_enable = 0xC000005B;
	kernel -> idt_mount( KERNEL_IDT_IRQ_offset + (module_usb_controller[ c ].irq_pin_and_line & STD_MASK_byte), KERNEL_IDT_TYPE_irq, (uintptr_t) module_usb_ohci_irq );
	kernel -> io_apic_connect( KERNEL_IDT_IRQ_offset + (module_usb_controller[ c ].irq_pin_and_line & STD_MASK_byte), KERNEL_IO_APIC_iowin + ((module_usb_controller[ c ].irq_pin_and_line & STD_MASK_byte) * 0x02) );

	// PORT(s) -------------------------------------------------------------

// MACRO_DEBUF();
// MACRO_DEBUF();

	uint32_t *port_status = (uint32_t *) ohci ->rh_port_status;
	volatile uint32_t test = port_status[ 0 ];
	test = port_status[ 1 ];

}