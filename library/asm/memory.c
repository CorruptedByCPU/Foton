/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void lib_asm_memory( struct LIB_ASM_STRUCTURE *asm ) {
	#ifdef DEBUF
		log( "{s%u,i%u,b%u}", asm -> sib.scale, asm -> sib.index, asm -> sib.base );
	#endif

	// start memory access
	log( "\033[0m[" );

	// there was something before displacement
	uint8_t semaphore = FALSE;

	// extended memory access?
	if( asm -> sib_semaphore ) {
		// base register
		if( asm -> modrm.mod || asm -> sib.base != 0x05 ) {
			// show
			log( "\033[38;2;255;166;87m%s", r[ asm -> mem_bits ][ asm -> sib.base | (asm -> rex.b << 3) ] );

			// ready
			semaphore = TRUE;
		}

		// index register
		if( asm -> sib.index != 0x04 ) {
			// base printed?
			if( semaphore ) log( "\033[0m + " );

			// index register
			log( "\033[38;2;255;166;87m%s", r[ asm -> mem_bits ][ asm -> sib.index | (asm -> rex.x << 3) ] );

			// with scale?
			if( asm -> sib.scale ) log( "\033[38;5;208m*%s", s[ asm -> sib.scale ] );

			// ready
			semaphore = TRUE;
		}

		// 32 bit displacement
		if( ! semaphore ) {
			// show
			if( asm -> displacement & STD_SIZE_DWORD_sign ) log( "\033[38;2;121;192;255m0x%8X", (uint64_t) asm -> displacement );
			else log( "\033[38;2;121;192;255m0x%8X", (uint32_t) asm -> displacement );

			// end memory access
			log( "\033[0m]" );

			// done
			return;
		}
	// no, simple one
	} else {
		// special case for 64 bit
		if( ! asm -> modrm.mod && asm -> modrm.rm == 0x05 )
			// relative address
			log( "\033[38;5;208mrip" );
		else {
			// default one
			log( "\033[38;2;255;166;87m%s", r[ asm -> mem_bits ][ asm -> modrm.rm | (asm -> rex.b << 3) ] );
		}

		// there are defaults
		semaphore = TRUE;
	}

	// displacement
	if( asm -> displacement ) {
		// value signed?
		uint8_t sign = FALSE;	// no

		// did we show any registers?
		if( semaphore ) {
			// for 4 Byte value
			if( asm -> displacement_size == STD_SIZE_DWORD_byte ) {
				// address
				if( asm -> displacement & STD_SIZE_DWORD_sign ) log( "\033[0m - \033[38;2;121;192;255m0x%8X", (uint32_t) -asm -> displacement );
				else log( "\033[0m + \033[38;2;121;192;255m0x%8X", (uint32_t) asm -> displacement );
			// 1 Byte
			} else {
				// address
				if( asm -> displacement & STD_SIZE_BYTE_sign ) log( "\033[0m - \033[38;2;121;192;255m0x%2X", (uint8_t) -asm -> displacement );
				else log( "\033[0m + \033[38;2;121;192;255m0x%2X", (uint8_t) asm -> displacement );
			}
		// no
		} else {
			// for 4 Byte value
			if( asm -> displacement_size == STD_SIZE_DWORD_byte ) {
				// backward or forward?
				if( asm -> displacement & STD_SIZE_DWORD_sign ) log( "\033[38;2;121;192;255m-0x%8X", (uint32_t) -asm -> displacement );
				else log( "\033[38;2;121;192;255m0x%8X", (uint32_t) asm -> displacement );
			// 1 Byte
			} else {
				// backward or forward?
				if( asm -> displacement & STD_SIZE_BYTE_sign ) log( "\033[38;2;121;192;255m-0x%2X", (uint8_t) -asm -> displacement );
				else log( "\033[38;2;121;192;255m0x%2X", (uint8_t) asm -> displacement );
			}
		}
	}

	// end memory access
	log( "\033[0m]" );
}
