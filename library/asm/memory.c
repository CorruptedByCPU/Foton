/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void lib_asm_memory( struct LIB_ASM_STRUCTURE *asm ) {
	// current bits
	uint8_t bits = asm -> mem_bits;

	// address size override?
	if( asm -> prefix == 0x66 ) bits = 1;	// 16 bit
	if( asm -> prefix == 0x67 ) bits = 2;	// 32 bit

	#ifdef DEBUF
		log( "{s%u,i%u,b%u}", asm -> sib.scale, asm -> sib.index, asm -> sib.base );
	#endif

	// start memory access
	log( "[" );

	// there was something before displacement
	uint8_t semaphore = FALSE;

	// extended memory access?
	if( asm -> sib_semaphore ) {
		// base register
		if( asm -> modrm.mod || asm -> sib.base != 0x05 ) {
			// show
			log( "%s", r[ bits ][ asm -> sib.base | (asm -> rex.b << 3) ] );

			// ready
			semaphore = TRUE;
		}

		// index register
		if( asm -> sib.index != 0x04 ) {
			// base printed?
			if( semaphore ) log( " + " );

			// index register
			log( "%s", r[ bits ][ asm -> sib.index | (asm -> rex.x << 3) ] );

			// with scale?
			if( asm -> sib.scale ) log( "*%s", s[ asm -> sib.scale ] );

			// ready
			semaphore = TRUE;
		}

		// 32 bit displacement
		if( ! semaphore ) {
			// show
			log( "0x%X", asm -> displacement );

			// end memory access
			log( "]" );

			// done
			return;
		}
	// no, simple one
	} else {
		// special case for 64 bit
		if( ! asm -> modrm.mod && asm -> modrm.rm == 0x05 )
			// relative address
			log( "rip" );
		else {
			// default one
			log( "%s", r[ bits ][ asm -> modrm.rm | (asm -> rex.b << 3) ] );
		}

		// there are defaults
		semaphore = TRUE;
	}

	// displacement
	if( asm -> displacement ) {
		// did we show any registers?
		if( semaphore ) {
			// address
			if( asm -> displacement & STD_SIZE_QWORD_sign ) log( " - 0x%X", (uint64_t) -asm -> displacement );
			else log( " + 0x%X", (uint64_t) asm -> displacement );
		// no
		} else {
			// backward or forward?
			if( asm -> displacement & STD_SIZE_QWORD_sign ) log( "-0x%X", (uint64_t) -asm -> displacement );
			else log( "0x%X", (uint64_t) asm -> displacement );
		}
	}

	// end memory access
	log( "]" );
}
