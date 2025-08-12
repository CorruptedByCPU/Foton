/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void lib_asm_immediate( struct LIB_ASM_STRUCTURE *asm ) {
	// any of operands contains immediate?
	if( ! (asm -> instruction.options & (I << (LIB_ASM_OPTION_FLAG_offset * asm -> col))) ) return;	// no

	// relative type of immediate?
	if( asm -> instruction.options & FE ) {
		// retrieve relative value
		int8_t value = *(asm -> rip++);

		// show absolute address of relative value
		log( "0x%16X", (uintptr_t) asm -> rip + value );
	} else {
		// current bits
		uint8_t bits = asm -> reg_bits;

		// strictly definied size for first operand?
		if( asm -> instruction.options & F1 ) {
			if( asm -> instruction.options & B ) bits = BYTE;
			if( asm -> instruction.options & W ) bits = WORD;
			if( asm -> instruction.options & D ) bits = DWORD;
			if( asm -> instruction.options & Q ) bits = QWORD;
		} else {
			// there is somewhere an error, and I cannot find it, so... exception :)
			if( asm -> opcode_0 == 0x6B ) {
				if( asm -> instruction.options & (B << (LIB_ASM_OPTION_FLAG_offset * asm -> col)) ) bits = BYTE;
				if( asm -> instruction.options & (W << (LIB_ASM_OPTION_FLAG_offset * asm -> col)) ) bits = WORD;
				if( asm -> instruction.options & (D << (LIB_ASM_OPTION_FLAG_offset * asm -> col)) ) bits = DWORD;
			}
		}

		// not in first column?
		if( asm -> col ) log( "\033[0m,\t" );

		// select print size
		switch( bits ) {
			case BYTE: {
				// show 1 Byte
				log( "\033[38;2;121;192;255m0x%2X", (uint8_t) *(asm -> rip++) );

				// done
				break;
			}

			case WORD: {
				// acquire 2 Bytes
				uint16_t immediate = *((uint16_t *) asm -> rip); asm -> rip += 2;

				// show
				log( "\033[38;2;121;192;255m0x%4X", immediate );

				// done
				break;
			}

			case DWORD: {
				// acquire 4 Bytes
				uint32_t immediate = (uint32_t) *((uint32_t *) asm -> rip); asm -> rip += 4;

				// show
				log( "\033[38;2;121;192;255m0x%8X", immediate );

				// done
				break;
			}
		}
	}
}
