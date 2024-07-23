/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void kernel_init_limine( void ) {
	// linear framebuffer is available (with 32 bits per pixel)?
	if( limine_framebuffer_request.response == NULL || ! limine_framebuffer_request.response -> framebuffer_count || limine_framebuffer_request.response -> framebuffers[ 0 ] -> bpp != STD_VIDEO_DEPTH_bit ) {
		// show error
		kernel_log( (uint8_t *) "I See a Darkness.\n" );

		// hold the door
		while( TRUE );
	}

	// memory map provided?
	if( limine_memmap_request.response == NULL || ! limine_memmap_request.response -> entry_count ) {
		// show error
		kernel_log( (uint8_t *) "Houston, we have a problem.\n" );

		// hold the door
		while( TRUE );
	}

	// RSDP pointer available?
	if( limine_rsdp_request.response == NULL || ! limine_rsdp_request.response -> address ) {
		// show error
		kernel_log( (uint8_t *) "Hello Darkness, My Old Friend.\n" );

		// hold the door
		while( TRUE );
	}

	// information about kernel?
	if( limine_kernel_file_request.response == NULL || limine_kernel_address_request.response == NULL ) {
		// show error
		kernel_log( (uint8_t *) "Whisky.\n" );

		// hold the door
		while( TRUE );
	}

	// modules attached?
	if( limine_module_request.response == NULL || ! limine_module_request.response -> module_count ) {
		// show error
		kernel_log( (uint8_t *) "Where Are My Testicles, Summer?\n" );

		// hold the door
		while( TRUE );
	}
}