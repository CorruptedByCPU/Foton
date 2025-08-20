/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void lib_asm_value( struct LIB_ASM_STRUCTURE *asm ) {
	// parse first value operand (and others, if exist)
	for( uint8_t i = 0; i < 3; i++ ) {
		// value?
		if( ! (asm -> instruction.options & (V << (i * LIB_ASM_OPTION_OPERAND_offset))) ) continue;	// no

		// add comma?
		if( asm -> comma_semaphore ) log( LIB_ASM_COLOR_DEFAULT","LIB_ASM_SEPARATOR );

		// current bits
		uint8_t bits = asm -> register_bits;

		// change to 16 bit mode?
		if( asm -> register_semaphore ) bits = WORD;

		// change to 64 bit mode? (only opcode 0xB8-0xBF, there is nothing else with 64 bit value)
		if( asm -> rex.w && asm -> opcode >= 0xB8 && asm -> opcode <= 0xBF ) bits = QWORD;

		// strictly definied size for operand?
		if( asm -> instruction.options & (F0 | F1 | F2) ) {
			// 8 bit
			if( asm -> instruction.options & (B << (i * LIB_ASM_OPTION_OPERAND_offset)) ) bits = BYTE;
			// 16 bit
			if( asm -> instruction.options & (W << (i * LIB_ASM_OPTION_OPERAND_offset)) ) bits = WORD;
			// 32 bit
			if( asm -> instruction.options & (D << (i * LIB_ASM_OPTION_OPERAND_offset)) ) bits = DWORD;
		}

		// retrieve value
		int64_t value;

		// show specific length
		switch( bits ) {
			case BYTE: {
				// retrieve value
				value = (int8_t) *(asm -> rip++);

				// relative type?
				if( asm -> instruction.options & FR ) log( LIB_ASM_COLOR_IMMEDIATE"0x%16X", (uintptr_t) asm -> rip + value );
				// no
				else log( LIB_ASM_COLOR_IMMEDIATE"0x%2X", value );

				// done
				break;
			}

			case WORD: {
				// retrieve value
				value = (int16_t) *((int16_t *) asm -> rip); asm -> rip += 2;

				// relative type?
				if( asm -> instruction.options & FR ) log( LIB_ASM_COLOR_IMMEDIATE"0x%16X", (uintptr_t) asm -> rip + value );
				// no
				else log( LIB_ASM_COLOR_IMMEDIATE"0x%4X", value );

				// done
				break;
			}

			case DWORD: {
				// retrieve value
				value = (int32_t) *((int32_t *) asm -> rip); asm -> rip += 4;

				// relative type?
				if( asm -> instruction.options & FR ) log( LIB_ASM_COLOR_IMMEDIATE"0x%16X", (uintptr_t) asm -> rip + value );
				// no
				else log( LIB_ASM_COLOR_IMMEDIATE"0x%8X", value );

				// done
				break;
			}

			case QWORD: {
				// retrieve value
				value = (int64_t) *((int64_t *) asm -> rip); asm -> rip += 8;

				// relative type?
				if( asm -> instruction.options & FR ) log( LIB_ASM_COLOR_IMMEDIATE"0x%16X", (uintptr_t) asm -> rip + value );
				// no
				else log( LIB_ASM_COLOR_IMMEDIATE"0x%16X", value );

				// done
				break;
			}
		}

		// separator required from now on
		asm -> comma_semaphore = TRUE;
	}
}
