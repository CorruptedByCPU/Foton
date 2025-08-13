/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void lib_asm_memory( struct LIB_ASM_STRUCTURE *asm ) {
	// add comma?
	if( asm -> comma_semaphore ) log( LIB_ASM_COLOR_DEFAULT","LIB_ASM_SEPARATOR );

	// start memory access
	log( LIB_ASM_COLOR_MEMORY"[" );

	// there was something before displacement
	uint8_t semaphore = FALSE;

	// extended memory access?
	if( asm -> sib.semaphore ) {
		// base register
		if( asm -> modrm.mod || asm -> sib.base != 0x05 ) {
			// show
			log( LIB_ASM_COLOR_REGISTER"%s", r[ asm -> memory_bits ][ asm -> sib.base | (asm -> rex.b << 3) ] );

			// ready
			semaphore = TRUE;
		}

		// index register
		if( asm -> sib.index != 0x04 ) {
			// base printed?
			if( semaphore ) log( LIB_ASM_COLOR_DEFAULT" + " );

			// index register
			log( LIB_ASM_COLOR_REGISTER"%s", r[ asm -> memory_bits ][ asm -> sib.index | (asm -> rex.x << 3) ] );

			// with scale?
			if( asm -> sib.scale ) log( LIB_ASM_COLOR_SCALE"*%s", s[ asm -> sib.scale ] );

			// ready
			semaphore = TRUE;
		}

		// 32 bit displacement
		if( ! semaphore ) {
			// show
			log( LIB_ASM_COLOR_IMMEDIATE"0x%8X", (uint64_t) asm -> displacement );

			// done
			goto	end;
		}
	// no, simple one
	} else {
		// special case for 64 bit
		if( ! asm -> modrm.mod && asm -> modrm.rm == 0x05 )
			// relative address
			log( LIB_ASM_COLOR_REGISTER"rip" );
		else {
			// default one
			log( LIB_ASM_COLOR_REGISTER"%s", r[ asm -> memory_bits ][ asm -> modrm.rm | (asm -> rex.b << 3) ] );
		}

		// there are defaults
		semaphore = TRUE;
	}

	// displacement
	if( asm -> displacement ) lib_asm_displacement( asm, semaphore );

end:
	// end memory access
	log( LIB_ASM_COLOR_MEMORY"]" );
}
