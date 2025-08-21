/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void lib_asm_memory( struct LIB_ASM_STRUCTURE *asm, uint8_t operand ) {
	// add comma?
	if( asm -> comma_semaphore ) log( LIB_ASM_COLOR_DEFAULT","LIB_ASM_SEPARATOR );

	// current bits
	uint8_t bits = asm -> register_bits;

	// change to 16 bit mode?
	if( asm -> register_semaphore ) bits = WORD;

	// change to 64 bit mode?
	if( ! asm -> register_semaphore && asm -> rex.w ) bits = QWORD;

	// strictly definied size for operand?
	if( asm -> instruction.options & (F0 | F1 | F2) ) {
		if( asm -> instruction.options & (B << (operand * LIB_ASM_OPTION_OPERAND_offset)) ) bits = BYTE;
		if( asm -> instruction.options & (W << (operand * LIB_ASM_OPTION_OPERAND_offset)) ) bits = WORD;
		if( asm -> instruction.options & (D << (operand * LIB_ASM_OPTION_OPERAND_offset)) ) bits = DWORD;
		// if( asm -> instruction.options & (Q << (operand * LIB_ASM_OPTION_OPERAND_offset)) ) bits = QWORD;
	}

	// start memory access
	log( LIB_ASM_COLOR_MEMORY"%s [", lib_asm_size[ bits ] );

	// there was something before displacement
	uint8_t relative = FALSE;

	// extended memory access?
	if( asm -> sib.semaphore ) {
		// current bits
		bits = asm -> memory_bits;

		// change to 32 bit mode?
		if( asm -> memory_semaphore ) bits = DWORD;

		// base register
		if( asm -> modrm.mod || asm -> sib.base != 0x05 ) {
			// show
			log( LIB_ASM_COLOR_REGISTER"%s", lib_asm_registers[ bits ][ asm -> sib.base | (asm -> rex.b << 3) ] );

			// ready
			relative = TRUE;
		}

		// index register
		if( asm -> sib.index != 0x04 ) {
			// base printed?
			if( relative ) log( LIB_ASM_COLOR_DEFAULT" + " );

			// index register
			log( LIB_ASM_COLOR_REGISTER"%s", lib_asm_registers[ bits ][ asm -> sib.index | (asm -> rex.x << 3) ] );

			// with scale?
			if( asm -> sib.scale ) log( LIB_ASM_COLOR_SCALE"*%s", lib_asm_scale[ asm -> sib.scale ] );

			// ready
			relative = TRUE;
		}
	// no, simple one
	} else {
		// special case of addressing
		if( asm -> modrm.mod == 0x00 && asm -> modrm.rm == 0x05 ) {
			// for 32 bit addressing mode
			if( asm -> memory_semaphore )
				// absolute address
				log( LIB_ASM_COLOR_IMMEDIATE"0x%8X", *((uint32_t *) asm -> rip ) );
			// for 64 bit addressing mode (default)
			else
				// relative address
				log( LIB_ASM_COLOR_IMMEDIATE"0x%16X", (uintptr_t) asm -> rip + *((int32_t *) asm -> rip) );
			
			// leave displacement
			asm -> rip += 4;
		// default one
		} else {
			// usage by: 0x83

			// 0xC6 exception
			if( asm -> opcode == 0xC6 ) {
				// current bits
				bits = asm -> register_bits;

				// change addressing mode?
				if( asm -> register_semaphore ) bits = DWORD;
			} else {
				// current bits
				bits = asm -> memory_bits;

				// change addressing mode?
				if( asm -> memory_semaphore ) bits = DWORD;
			}

			// show register
			log( LIB_ASM_COLOR_REGISTER"%s", lib_asm_registers[ bits ][ asm -> modrm.rm | (asm -> rex.b << 3) ] );
		}
		
		// there are defaults
		relative = TRUE;
	}

	// displacement
	if( asm -> displacement ) lib_asm_displacement( asm, relative );

end:
	// end memory access
	log( LIB_ASM_COLOR_MEMORY"]" );

	// separator required from now on
	asm -> comma_semaphore = TRUE;
}
