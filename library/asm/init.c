/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t lib_asm_init( struct LIB_ASM_STRUCTURE *asm ) {
	// no REX available by default
	// no ModR/M available by default
	// no SIB available by default
	// no Displacement available by default

	// we do not store information about Immediate/Relative/Offset values
	// they will be obtained by instruction table interpretation

	// we are working inside 64 bit CPU mode, so...
	asm -> memory_bits = QWORD;	// memory access is 64 bit by default
	asm -> register_bits = DWORD;	// but registers are 32 bit
	
	// their size may change depending on the prefixes(opcode: 0x66, 0x67) and REX.W

	// obtain all available control opcodes
	// until beginning of instruction
	while( TRUE ) {
		// obtain first opcode
		asm -> opcode = *(asm -> rip++);

		// ignore opcodes: null or invalid in 64 bit mode
		// http://ref.x86asm.net/coder64.html
		if( asm -> opcode == 0x06 || asm -> opcode == 0x07 || asm -> opcode == 0x0E || asm -> opcode == 0x16 || asm -> opcode == 0x17 || asm -> opcode == 0x1E || asm -> opcode == 0x1F || asm -> opcode == 0x27 || asm -> opcode == 0x26 || asm -> opcode == 0x2E || asm -> opcode == 0x2F || asm -> opcode == 0x36 || asm -> opcode == 0x37 || asm -> opcode == 0x3E || asm -> opcode == 0x3F || asm -> opcode == 0x60 || asm -> opcode == 0x61 || asm -> opcode == 0x62 || asm -> opcode == 0x82 || asm -> opcode == 0x9A || asm -> opcode == 0xC4 || asm -> opcode == 0xC5 || asm -> opcode == 0xD4 || asm -> opcode == 0xD5 || asm -> opcode == 0xD6 || asm -> opcode == 0xEA ) return FALSE;

		// segment descriptors FS and GS doesn't exist in my OS
		if( asm -> opcode == 0x64 || asm -> opcode == 0x65 ) continue;

		// operand size override?
		if( asm -> opcode == 0x66 ) {
			#ifdef DEBUF
				log( "{r} " );
			#endif
			
			// change registers size to 16 Bit
			asm -> register_semaphore = TRUE;
			
			// next prefix
			continue;
		}

		// memory addressing size override?
		if( asm -> opcode == 0x67 ) {
			#ifdef DEBUF
				log( "{m} " );
			#endif
			
			// change memory addressing to 32 bit
			asm -> memory_semaphore = TRUE;
			
			// next prefix
			continue;
		}

		// exclusive memory access?
		if( asm -> opcode == 0xF0 ) { log( LIB_ASM_COLOR_INSTRUCTION"lock\t" ); continue; }

		// REP
		// // if( first == 0xF2 ) { log( "repnz\t" ); continue; }	// or REPNE?
		if( asm -> opcode == 0xF3 ) {
			// pause instruction?
			if( *(asm -> rip) == 0x90 ) {
				// get instruction properties
				asm -> instruction = lib_asm_instruction_pause[ 0 ];

				// leave next opcode
				asm -> rip++;

				// end of initialization
				return TRUE;
			}

			// no, simple rep instruction
			log( LIB_ASM_COLOR_INSTRUCTION"rep\t" );

			// next opcode
			continue;
		}

		// REX
		if( (asm -> opcode & ~STD_MASK_byte_half) == 0x40 ) {
			// 64 bit operand size
			asm -> rex.w = (asm -> opcode >> 3) & TRUE;

			// extension of ModR/M registry field
			asm -> rex.r = (asm -> opcode >> 2) & TRUE;

			// extension of SIB index
			asm -> rex.x = (asm -> opcode >> 1) & TRUE;

			// extension of ModR/M r/m or SIB base field
			asm -> rex.b = asm -> opcode & TRUE;

			// change behavior of something :)
			asm -> rex.semaphore = TRUE;

			#ifdef DEBUF
				log( "(w%ur%ux%ub%u) ", asm -> rex.w, asm -> rex.r, asm -> rex.x, asm -> rex.b );
			#endif

			// ignore this REX if not last one

			// continue
			continue;
		}

		// done with prefixes
		break;
	}

	// acquired opcode of NOP?
	if( asm -> opcode == 0x90 && ! asm -> rex.semaphore && ! asm -> register_semaphore ) asm -> instruction = lib_asm_instruction_nop[ 0 ];
	else
		// get instruction properties from main table
		asm -> instruction = lib_asm_instruction_master[ asm -> opcode ];

	// 2-Byte asm -> opcode?
	if( asm -> opcode == 0x0F ) {
		// to do
	}

	// unknown instruction?
	if( ! asm -> instruction.name && ! asm -> instruction.group ) return FALSE;	// yes, end of interpretation

	// ModR/M exist for this mnemonic?
	if( asm -> instruction.options & FM ) {	// yes
		// obtain opcode
		uint8_t modrm = *(asm -> rip++);

		// adressing mode
		asm -> modrm.mod = modrm >> 6;

		// register asm -> opcode extension
		asm -> modrm.reg = (modrm >> 3) & 0x07;

		// register memory operand
		asm -> modrm.rm = modrm & 0x07;

		#ifdef DEBUF
			log( "(mod%ureg%urm%u) ", asm -> modrm.mod, asm -> modrm.reg, asm -> modrm.rm );
		#endif

		// memory manipulation?
		if( asm -> modrm.mod != 0x03 ) {
			// SIB exist for this mnemonic?
			if( asm -> modrm.rm == 0x04 ) {
				// obtain opcode
				uint8_t sib = *(asm -> rip++);

				// multipler for index
				asm -> sib.scale = sib >> 6;

				// register
				asm -> sib.index = (sib >> 3) & 7;

				// register
				asm -> sib.base = sib & 7;

				#ifdef DEBUF
					log( "(s%ui%ub%u) ", asm -> sib.scale, asm -> sib.index, asm -> sib.base );
				#endif

				// set semaphore
				asm -> sib.semaphore = TRUE;
			}
		}

		// set semaphore
		asm -> modrm.semaphore = TRUE;
	}

	// displacement at end of instruction?
	if( asm -> modrm.mod == 0x00 && asm -> sib.base == 0x05 ) { asm -> displacement = (int32_t) *((uint32_t *) asm -> rip); asm -> rip += 4; }
	if( asm -> modrm.mod == 0x01 ) { asm -> displacement = (int8_t) *(asm -> rip++); }
	if( asm -> modrm.mod == 0x02 ) { asm -> displacement = (int32_t) *((uint32_t *) asm -> rip); asm -> rip += 4; }

	// everything prepared
	return TRUE;
}
