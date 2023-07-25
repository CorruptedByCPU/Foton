/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_lapic_init() {
	// turn off Task Priority and Priority Sub-Class
	kernel -> lapic_base_address -> tp = EMPTY;

	// turn on Flat Mode
	kernel -> lapic_base_address -> df = KERNEL_LAPIC_DF_FLAG_flat_mode;

	// all logical/BSP processors gets interrupts (physical!)
	kernel -> lapic_base_address -> ld = KERNEL_LAPIC_LD_FLAG_target_cpu;

	// enable APIC controller on the BSP/logical processor
	kernel -> lapic_base_address -> siv = kernel -> lapic_base_address -> siv | KERNEL_LAPIC_SIV_FLAG_enable_apic | KERNEL_LAPIC_SIV_FLAG_spurious_vector3;

	// turn on internal interrupts time on APIC controller of BSP/logical processor
	kernel -> lapic_base_address -> lvt = kernel -> lapic_base_address -> lvt & ~KERNEL_LAPIC_LVT_TR_FLAG_mask_interrupts;

	// number of hardware interrupt at the end of the timer
	kernel -> lapic_base_address -> lvt = KERNEL_LAPIC_IRQ_number;

	// countdown time converter
	kernel -> lapic_base_address -> tdc = KERNEL_LAPIC_TDC_divide_by_1;
}
