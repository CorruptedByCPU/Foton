/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_apic( void ) {
	// turn off Task Priority and Priority Sub-Class
	kernel -> apic_base_address -> tp = EMPTY;

	// turn on Flat Mode
	kernel -> apic_base_address -> df = KERNEL_APIC_DF_FLAG_flat_mode;

	// all logical/BSP processors gets interrupts (physical!)
	kernel -> apic_base_address -> ld = KERNEL_APIC_LD_FLAG_target_cpu;

	// enable APIC controller on the BSP/logical processor
	kernel -> apic_base_address -> siv = kernel -> apic_base_address -> siv | KERNEL_APIC_SIV_FLAG_enable_apic | KERNEL_APIC_SIV_FLAG_spurious_vector;

	// turn on internal interrupts time on APIC controller of BS/A processor
	kernel -> apic_base_address -> lvt = kernel -> apic_base_address -> lvt & ~KERNEL_APIC_LVT_TR_FLAG_mask_interrupts;

	// number of hardware interrupt at the end of the timer
	kernel -> apic_base_address -> lvt = KERNEL_APIC_IRQ_number;

	// countdown time converter
	kernel -> apic_base_address -> tdc = KERNEL_APIC_TDC_divide_by_1;
}
