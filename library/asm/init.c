/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t lib_asm_init( struct LIB_ASM_STRUCTURE *asm ) {
	// no REX available by default
	asm -> rex_semaphore = asm -> rex.w = asm -> rex.r = asm -> rex.x = asm -> rex.b = EMPTY;

	// no ModR/M available by default
	asm -> modrm_semaphore = asm -> modrm.mod = asm -> modrm.reg = asm -> modrm.rm = EMPTY;

	// no SIB available by default
	asm -> sib_semaphore = asm -> sib.scale = asm -> sib.index = asm -> sib.base = EMPTY;

	// no Displacement/Immediate available by default
	asm -> displacement = EMPTY;

	// we are working inside 64 bit CPU mode
	asm -> reg_bits = 2;	// but default for registers are 32 bit
	asm -> mem_bits = 3;

	// obtain all available control opcodes
	// until end of instruction
	while( TRUE ) {
		// obtain opcode
		asm -> opcode_0 = *( asm -> rip++ );

		// ignore opcodes: null or invalid in 64 bit mode
		// http://ref.x86asm.net/coder64.html
		if( asm -> opcode_0 == 0x06 || asm -> opcode_0 == 0x07 || asm -> opcode_0 == 0x0E || asm -> opcode_0 == 0x16 || asm -> opcode_0 == 0x17 || asm -> opcode_0 == 0x1E || asm -> opcode_0 == 0x1F || asm -> opcode_0 == 0x27 || asm -> opcode_0 == 0x26 || asm -> opcode_0 == 0x2E || asm -> opcode_0 == 0x2F || asm -> opcode_0 == 0x36 || asm -> opcode_0 == 0x37 || asm -> opcode_0 == 0x3E || asm -> opcode_0 == 0x3F || asm -> opcode_0 == 0x60 || asm -> opcode_0 == 0x61 || asm -> opcode_0 == 0x62 || asm -> opcode_0 == 0x82 || asm -> opcode_0 == 0x9A || asm -> opcode_0 == 0xC4 || asm -> opcode_0 == 0xC5 || asm -> opcode_0 == 0xD4 || asm -> opcode_0 == 0xD5 || asm -> opcode_0 == 0xD6 || asm -> opcode_0 == 0xEA ) return FALSE;

		// select different descriptor
		if( asm -> opcode_0 == 0x64 ) { asm -> descriptor = 'f'; continue; }
		if( asm -> opcode_0 == 0x65 ) { asm -> descriptor = 'g'; continue; }

		// change size?
		if( asm -> opcode_0 == 0x66 ) { asm -> prefix = asm -> opcode_0; continue; }	// bit
		if( asm -> opcode_0 == 0x67 ) { asm -> prefix = asm -> opcode_0; continue; }	// bit

		// 0x9B (x87fpu)

		// exclusive memory access?
		if( asm -> opcode_0 == 0xF0 ) { log( "lock\t" ); continue; }

		// // REP
		// // if( first == 0xF2 ) { log( "repnz\t" ); continue; }	// or REPNE?
		// // if( first == 0xF3 ) { log( "repe\t" ); continue; }	// or REPZ? | SSE
		// if( first == 0xF2 || first == 0xF3 ) { log( "%2X ", (uint8_t) first ); continue; }

		// REX
		if( (asm -> opcode_0 & ~STD_MASK_byte_half) == LIB_ASM_FLAG_REX_base_address ) {
			// change behavior of something :)
			asm -> rex_semaphore = asm -> opcode_0;

			// 64 bit operand size
			asm -> rex.w = (asm -> opcode_0 >> 3) & TRUE;

			// extension of ModR/M registry field
			asm -> rex.r = (asm -> opcode_0 >> 2) & TRUE;

			// extension of SIB index
			asm -> rex.x = (asm -> opcode_0 >> 1) & TRUE;

			// extension of ModR/M r/m or SIB base field
			asm -> rex.b = asm -> opcode_0 & TRUE;

			// ignore this REX if not last one

			// continue
			continue;
		}

		// done with prefixes
		break;
	}

	// get instruction properties
	asm -> instruction = i[ asm -> opcode_0 ];

	// 2-Byte asm -> opcode_0?
	if( asm -> opcode_0 == 0x0F ) {
		// to do
	}

	// unknown instruction?
	if( ! asm -> instruction.name ) return FALSE;	// yes, end of interpretation

	// ModR/M exist for this mnemonic?
	if( asm -> instruction.options & FM ) {	// yes
		// obtain opcode
		asm -> modrm_semaphore = *(asm -> rip++);

		// adressing mode
		asm -> modrm.mod = (asm -> modrm_semaphore >> 6);

		// register asm -> opcode_0 extension
		asm -> modrm.reg = (asm -> modrm_semaphore >> 3) & 7;

		// register memory operand
		asm -> modrm.rm = asm -> modrm_semaphore & 7;

		// memory manipulation?
		if( asm -> modrm.mod != 0x03 ) {
			// SIB exist for this mnemonic?
			if( asm -> modrm.rm == 0x04 ) {
				// obtain opcode
				asm -> sib_semaphore = *(asm -> rip++);

				// multipler for index
				asm -> sib.scale = asm -> sib_semaphore >> 6;

				// register
				asm -> sib.index = (asm -> sib_semaphore >> 3) & 7;

				// register
				asm -> sib.base = asm -> sib_semaphore & 7;

				// set semaphore
				asm -> sib_semaphore = TRUE;
			}
		}

		// set semaphore
		asm -> modrm_semaphore = TRUE;
	}

	// displacement at end of instruction?
	if( asm -> modrm.mod == 0x00 && asm -> sib.base == 0x05 ) { asm -> displacement = *((uint32_t *) asm -> rip); asm -> rip += 4; }
	if( asm -> modrm.mod == 0x01 ) { asm -> displacement = *(asm -> rip++); }
	if( asm -> modrm.mod == 0x02 ) { asm -> displacement = *((uint32_t *) asm -> rip); asm -> rip += 4; }

	// everything prepared
	return TRUE;
}
