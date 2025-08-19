/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void lib_asm_memory( struct LIB_ASM_STRUCTURE *asm ) {
	// add comma?
	if( asm -> comma_semaphore ) log( LIB_ASM_COLOR_DEFAULT","LIB_ASM_SEPARATOR );

	// start memory access
	log( LIB_ASM_COLOR_MEMORY"[" );

	// there was something before displacement
	uint8_t relative = FALSE;

	// extended memory access?
	if( asm -> sib.semaphore ) {
		// base register
		if( asm -> modrm.mod || asm -> sib.base != 0x05 ) {
			// show
			log( LIB_ASM_COLOR_REGISTER"%s", r[ asm -> memory_bits ][ asm -> sib.base | (asm -> rex.b << 3) ] );

			// ready
			relative = TRUE;
		}

		// index register
		if( asm -> sib.index != 0x04 ) {
			// base printed?
			if( relative ) log( LIB_ASM_COLOR_DEFAULT" + " );

			// index register
			log( LIB_ASM_COLOR_REGISTER"%s", r[ asm -> memory_bits ][ asm -> sib.index | (asm -> rex.x << 3) ] );

			// with scale?
			if( asm -> sib.scale ) log( LIB_ASM_COLOR_SCALE"*%s", s[ asm -> sib.scale ] );

			// ready
			relative = TRUE;
		}

		// 32 bit displacement
		if( ! relative ) {
			// show
			log( LIB_ASM_COLOR_IMMEDIATE"0x%8X", (uint64_t) asm -> displacement );

			// done
			goto	end;
		}
	// no, simple one
	} else {
		// special case of addressing
		if( asm -> modrm.mod == 0x00 && asm -> modrm.rm == 0x05 ) {
			// for 32 bit CPU mode
			if( asm -> memory_bits == DWORD ) {
				// retrieve address
				uint32_t address = (uint32_t) *((uint32_t *) asm -> rip); asm -> rip += 4;

				// absolute address
				log( LIB_ASM_COLOR_IMMEDIATE"0x%8X", (uint32_t) address );
			// for 64 bit CPU mode
			} else {
				// // if base doesn't exist
				// if( ! asm -> sib.base ) {
				// 	// absolute address
				// 	address = *((uint32_t *) asm -> rip);
				// 	log( LIB_ASM_COLOR_IMMEDIATE"0x%8X", address );
				// } else {
					// relative address
					log( LIB_ASM_COLOR_IMMEDIATE"0x%16X", (uint64_t) asm -> rip + (int32_t) *((int32_t *) asm -> rip) );
				// }

				// leave displacement
				asm -> rip += 4;
			}
		// default one
		} else log( LIB_ASM_COLOR_REGISTER"%s", r[ asm -> memory_bits ][ asm -> modrm.rm | (asm -> rex.b << 3) ] );

		// there are defaults
		relative = TRUE;
	}

	// displacement
	if( asm -> displacement ) lib_asm_displacement( asm, relative );

end:
	// end memory access
	log( LIB_ASM_COLOR_MEMORY"]" );
}
