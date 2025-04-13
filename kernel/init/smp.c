/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_smp( void ) {
	// other CPUs are available?
	if( limine_smp_request.response == EMPTY ) return;	// no

	// amount of running APs
	uint64_t count = INIT;

	// initialize AP one by one
	for( uint64_t i = INIT; i < limine_smp_request.response -> cpu_count; i++ ) {
		// do not reload BSP processor, yet
		if( limine_smp_request.response -> cpus[ i ] -> lapic_id == kernel_apic_id() ) continue;

		// set first function to execute by AP
		limine_smp_request.response -> cpus[ i ] -> goto_address = kernel_init_ap;

		// AP is initializing
		count++;

		// wait for each AP initialization to finish

		// it's very important to wait :) because all additional APs
		// will use the same stack pointer as BSP
		while( kernel -> cpu_limit < count );

		// remember CPU ID if greater than previous one
		if( kernel -> apic_id_last < limine_smp_request.response -> cpus[ i ] -> lapic_id ) kernel -> apic_id_last = limine_smp_request.response -> cpus[ i ] -> lapic_id;

		//--------------------------------------------------------------
		// no support for CPU clusters, yet
		//--------------------------------------------------------------
	}
}
