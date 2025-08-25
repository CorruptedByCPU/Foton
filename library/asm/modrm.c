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

			// source, only if instruction doesn't belong to group
			if( ! asm -> instruction.group ) lib_asm_register( asm, 1, register_reg );
		// by default
		} else {
			// exception
			if( asm -> opcode == 0xFF ) {
				// for sub instructions "call" and further
				if( asm -> modrm.reg == 0x02 || asm -> modrm.reg == 04 || asm -> modrm.reg == 0x06 )
					// destination
					log( LIB_ASM_COLOR_REGISTER"%s", lib_asm_registers[ QWORD ][ asm -> modrm.rm ] );
			} else {
				// destination
				lib_asm_register( asm, 0, register_reg );

				// source, only if instruction doesn't belong to group
				// and doesn't have second register as operand
				if( ! asm -> instruction.group && ! (asm -> instruction.options & FE) ) lib_asm_register( asm, 1, register_rm );
			}
		}
	// memory addressing mode
	} else {
		// only memory access?
		if( asm -> instruction.options & FE ) {
			// exception, for sub instructions "call" and further
			if( asm -> opcode == 0xFF && (asm -> modrm.reg == 0x02 || asm -> modrm.reg == 04 || asm -> modrm.reg == 0x06 ) )
				// only 64 bit memory access
				asm -> register_bits = QWORD;

			// destination
			lib_asm_memory( asm, 0 );

			// done
			return;
		}

		// memory access first?
		if( asm -> instruction.options & M ) {
			// destination
			lib_asm_memory( asm, 0 );

			// source, only if instruction doesn't belong to group
			if( ! asm -> instruction.group ) lib_asm_register( asm, 1, register_reg );
		// no, second
		} else {
			// destination
			lib_asm_register( asm, 0, register_reg );

			// source
			lib_asm_memory( asm, 1 );
		}
	}

	// supplement value/register
	if( asm -> opcode == 0xD0 || asm -> opcode == 0xD1 ) log( LIB_ASM_COLOR_DEFAULT","LIB_ASM_SEPARATOR""LIB_ASM_COLOR_IMMEDIATE"0x01" );
	if( asm -> opcode == 0xD2 || asm -> opcode == 0xD3 ) log( LIB_ASM_COLOR_DEFAULT","LIB_ASM_SEPARATOR""LIB_ASM_COLOR_REGISTER"cl" );

	// separator required from now on
	asm -> comma_semaphore = TRUE;
}
