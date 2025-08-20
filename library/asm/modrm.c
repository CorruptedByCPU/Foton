/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void lib_asm_modrm( struct LIB_ASM_STRUCTURE *asm ) {
	// calculate source/destination opcodes
	uint8_t register_reg = asm -> modrm.reg | (asm -> rex.r << 3);
	uint8_t register_rm = asm -> modrm.rm | (asm -> rex.b << 3);

	// direct register addressing mode
	if( asm -> modrm.mod == 0x03 ) {
		// invert source/destination?
		if( asm -> instruction.options & FV ) {
			// thats how instruction is designed

			// destination
			lib_asm_register( asm, 0, register_rm );

			// separator required from now on
			asm -> comma_semaphore = TRUE;

			// source
			lib_asm_register( asm, 1, register_reg );
		// by default
		} else {
			// destination
			lib_asm_register( asm, 0, register_reg );

			// separator required from now on
			asm -> comma_semaphore = TRUE;

			// source
			lib_asm_register( asm, 1, register_rm );
		}
	// memory addressing mode
	} else {
		// memory access first?
		if( asm -> instruction.options & M ) {
			// destination
			lib_asm_memory( asm, 0 );

			// separator required from now on
			asm -> comma_semaphore = TRUE;

			// source
			lib_asm_register( asm, 1, register_reg );
		// no, second
		} else {
			// destination
			lib_asm_register( asm, 0, register_reg );

			// separator required from now on
			asm -> comma_semaphore = TRUE;

			// source
			lib_asm_memory( asm, 1 );

			// // exception
			// // there is no source register for opcode 0x8F (pop)
			// if( asm -> opcode != 0x8F ) {
			// 	// destination
			// 	lib_asm_register( asm, 0, register_reg );

			// 	// separator required from now on
			// 	asm -> comma_semaphore = TRUE;
			// } else {
			// 	// is this code below, really required?
			// 	// I never saw 16 bit POP in 64 bit CPU mode

			// 	// current bits
			// 	uint8_t bits = asm -> register_bits;

			// 	// opcode cannot be 32 bit in 64 bit mode CPU
			// 	if( bits == DWORD ) bits = QWORD;

			// 	// prefix
			// 	log( LIB_ASM_COLOR_MEMORY"%s ", size[ bits ] );
			// }

			// // source
			// lib_asm_memory( asm );
		}
	}
}
