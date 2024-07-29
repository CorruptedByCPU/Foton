/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_smp( void ) {
	// other CPUs are available?
	if( limine_smp_request.response == NULL ) return;	// no

	// amount of running APs
	uint64_t local_smp_count = EMPTY;

	// initialize every Local CPU
	for( uint64_t i = 0; i < limine_smp_request.response -> cpu_count; i++ ) {
		// do not reload BSP processor, yet
		if( limine_smp_request.response -> cpus[ i ] -> lapic_id == kernel_lapic_id() ) continue;

		// set first function to execute by Local CPU
		limine_smp_request.response -> cpus[ i ] -> goto_address = (void *) &kernel_init_ap;

		// AP is initializing
		local_smp_count++;

		// wait for each AP initialization to finish

		// it's very important to wait :) because all additional CPUs at initialization
		// will use the same stack pointer as BSP
		while( local_smp_count > kernel -> cpu_count );

		// remember CPU ID if greater than previous one
		if( kernel -> lapic_id_highest < limine_smp_request.response -> cpus[ i ] -> lapic_id ) kernel -> lapic_id_highest = limine_smp_request.response -> cpus[ i ] -> lapic_id;

		//--------------------------------------------------------------
		// no support for CPU clusters, yet
		//--------------------------------------------------------------

		// CPU limit acquired?
		if( kernel -> cpu_count >= 8 || i >= 8 ) break;	// yes
	}
}