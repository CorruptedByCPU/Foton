/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void lib_asm_immediate( struct LIB_ASM_STRUCTURE *asm ) {
	// current bits
	uint8_t bits = asm -> reg_bits;

	// strictly definied size for first operand?
	if( asm -> instruction.options & F1 ) {
		if( asm -> instruction.options & B ) bits = BYTE;
		if( asm -> instruction.options & W ) bits = WORD;
		if( asm -> instruction.options & D ) bits = DWORD;
		// if( asm -> instruction.options & Q ) bits = QWORD;
	}

	// select print size
	switch( bits ) {
		case BYTE: {
			// show 1 Byte
			log( "0x%2X", (uint8_t) *(asm -> rip++) );

			// done
			break;
		}

		case WORD: {
			// acquire 2 Bytes
			uint16_t immediate = *((uint16_t *) asm -> rip); asm -> rip += 2;

			// show
			log( "0x%4X", immediate );

			// done
			break;
		}

		case DWORD: {
			// acquire 4 Bytes
			uint32_t immediate = *((uint32_t *) asm -> rip); asm -> rip += 4;

			// show
			log( "0x%8X", immediate );

			// done
			break;
		}
	}
}
