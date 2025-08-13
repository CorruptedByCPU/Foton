/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t lib_asm_modrm( struct LIB_ASM_STRUCTURE *asm ) {
	// ModR/M exist?
	if( asm -> modrm.semaphore ) {
		// calculate source/destination opcodes
		uint8_t register_reg = asm -> modrm.reg | (asm -> rex.r << 3);
		uint8_t register_rm = asm -> modrm.rm | (asm -> rex.b << 3);

		// direct register addressing mode
		if( asm -> modrm.mod == 0x03 ) {
			// invert source/destination?
			if( asm -> instruction.options & FI ) {
				// thats how instruction is design
				register_reg = register_rm;
				register_rm = asm -> modrm.reg | (asm -> rex.r << 3);
			}

			// show destination first
			lib_asm_register( asm, 0, register_reg );
			asm -> comma_semaphore = TRUE;
			lib_asm_register( asm, 1, register_rm );
		// memory addressing mode
		} else {
			// show destination
			if( asm -> instruction.options & M ) {
				// show
				lib_asm_memory( asm );
				asm -> comma_semaphore = TRUE;
				lib_asm_register( asm, 1, register_reg );
			// and source
			} else {
				// show
				lib_asm_register( asm, 0, register_reg );
				asm -> comma_semaphore = TRUE;
				lib_asm_memory( asm );
			}
		}
	}
	
	// instruction without ModR/M
	else return FALSE;

	// ModR/M parsed
	return TRUE;
}
