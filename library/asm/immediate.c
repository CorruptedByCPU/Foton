/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void lib_asm_immediate( struct LIB_ASM_STRUCTURE *asm ) {
	// parse first immediate operand
	for( uint8_t i = 0; i < 3; i++ ) {
		// immediate?
		if( ! (asm -> instruction.options & (I << (i * LIB_ASM_OPTION_FLAG_offset))) ) continue;	// no

		// default size of immediate is same as destination register
		uint8_t bits = asm -> register_bits;

		// register size override by REX?
		if( asm -> rex.w ) bits = QWORD;	// forced 64 bit

		// strictly definied size?
		if( asm -> instruction.options & FO ) {
			if( asm -> instruction.options & (B << (i * LIB_ASM_OPTION_FLAG_offset)) ) bits = BYTE;
			if( asm -> instruction.options & (W << (i * LIB_ASM_OPTION_FLAG_offset)) ) bits = WORD;
			if( asm -> instruction.options & (D << (i * LIB_ASM_OPTION_FLAG_offset)) ) bits = DWORD;
			// if( asm -> instruction.options & (Q << (i * LIB_ASM_OPTION_FLAG_offset)) ) bits = QWORD;
		// exception for opcode: 0xC1 0xE5
		} else if( asm -> opcode == 0xC1 || asm -> opcode == 0xE5 ) {
			if( asm -> instruction.options & (B << (i * LIB_ASM_OPTION_FLAG_offset)) ) bits = BYTE;
		}

		// add comma?
		if( asm -> comma_semaphore ) log( LIB_ASM_COLOR_DEFAULT","LIB_ASM_SEPARATOR );

		// relative type of value?
		if( asm -> instruction.options & FR ) {
			// show specific length
			if( bits == BYTE ) {
				// retrieve value
				int8_t relative = *(asm -> rip++);
				log( LIB_ASM_COLOR_IMMEDIATE"0x%16X", (uintptr_t) asm -> rip + relative );
			}
			if( bits == WORD ) {
				// retrieve value
				int16_t relative = (uint16_t) *((uint16_t *) asm -> rip); asm -> rip += 2;
				log( LIB_ASM_COLOR_IMMEDIATE"0x%16X", (uintptr_t) asm -> rip + relative );
			}
			if( bits == DWORD ) {
				// retrieve value
				int32_t relative = (uint32_t) *((uint32_t *) asm -> rip); asm -> rip += 4;
				log( LIB_ASM_COLOR_IMMEDIATE"0x%16X", (uintptr_t) asm -> rip + relative );
			}
			if( bits == QWORD ) {
				// retrieve value
				int64_t relative = (uint64_t) *((uint64_t *) asm -> rip); asm -> rip += 8;
				log( LIB_ASM_COLOR_IMMEDIATE"0x%16X", (uintptr_t) asm -> rip + relative );
			}
		} else {
			// retrieve immediate value
			uint64_t immediate;

			// show specific length
			if( bits == BYTE ) {
				// retrieve value
				immediate = *(asm -> rip++);
				log( LIB_ASM_COLOR_IMMEDIATE"0x%2X", immediate );
			}
			if( bits == WORD ) {
				// retrieve value
				immediate = (uint16_t) *((uint16_t *) asm -> rip); asm -> rip += 2;
				log( LIB_ASM_COLOR_IMMEDIATE"0x%4X", immediate );
			}
			if( bits == DWORD ) {
				// retrieve value
				immediate = (int32_t) *((uint32_t *) asm -> rip); asm -> rip += 4;
				if( asm -> rex.w ) log( LIB_ASM_COLOR_IMMEDIATE"0x%16X", (int64_t) immediate );
				else log( LIB_ASM_COLOR_IMMEDIATE"0x%8X", immediate );
			}
			if( bits == QWORD ) {
				// retrieve value
				immediate = (uint64_t) *((uint64_t *) asm -> rip); asm -> rip += 8;
				log( LIB_ASM_COLOR_IMMEDIATE"0x%16X", immediate );
			}
		}

		// separator required from now on
		asm -> comma_semaphore = TRUE;
	}
}
