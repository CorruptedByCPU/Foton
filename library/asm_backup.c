 	log( "%2X|%s\t", asm.opcode_0, asm.instruction.name );

	// only instruction name?
	if( ! asm.instruction.options ) return (uintptr_t) asm.rip - (uintptr_t) rip;	// yeah

	// register-direct mode?
	if( asm.modrm.mod == 0x03 )	{	// 0b11
		log( "{1}" );

		uint8_t *operand_0 = lib_asm_register( (struct LIB_ASM_STRUCTURE *) &asm, 0, R, asm.modrm.reg | (asm.rex.r << 3) );
		uint8_t *operand_1 = lib_asm_register( (struct LIB_ASM_STRUCTURE *) &asm, 1, R, asm.modrm.rm | (asm.rex.b << 3) );
		uint8_t *operand_n = EMPTY;

		// operand 2 size strictly definied?
		if( asm.instruction.options & FO ) {	// yes
			// select definied operand size
			uint8_t bits;
			if( asm.instruction.options & (B << 7) ) bits = 0;	// 8 bit
			if( asm.instruction.options & (W << 7) ) bits = 1;	// 16 bit
			if( asm.instruction.options & (D << 7) ) bits = 2;	// 32 bit
			if( asm.instruction.options & (Q << 7) ) bits = 3;	// 64 bit

			// select register name
			operand_n = r[ bits ][ asm.modrm.rm | (asm.rex.b << 3) ];
		}

		if( asm.instruction.options & FO ) log( "%s\t%s", operand_0, operand_n );
		else log( "%s,\t%s", operand_1, operand_0 );
	} else {
		log( "{2}" );

		// push 0x50-0x5F
		if( asm.instruction.options & FR ) {
			log( "{2.1}" );
			log( "%s", lib_asm_register( (struct LIB_ASM_STRUCTURE *) &asm, 0, R, asm.opcode_0 & STD_MASK_byte_half | (asm.rex.b << 3) ));
		// push 0x68, 0x6A
		} else if( asm.instruction.options & I ) {
			log( "{2.2}" );
			uint32_t immediete;
			if( asm.instruction.options & B ) immediete = *(asm.rip++);
			if( asm.instruction.options & D ) { immediete = *((uint32_t *) asm.rip); asm.rip += 4; }
			
			log( "0x%X", immediete );
		// memory operation
		} else {
			log( "{2.3}" );
			if( asm.modrm_semaphore && ! asm.modrm.mod && ! asm.modrm.rm ) {
				log( "{2.3.1}" );
				if( asm.instruction.options & M ) {
					log( "{2.3.1.1}" );
					log( "[%s],\t", lib_asm_register( (struct LIB_ASM_STRUCTURE *) &asm, 0, M, asm.modrm.rm | (asm.rex.b << 3) ) );					
					log( "%s", lib_asm_register( (struct LIB_ASM_STRUCTURE *) &asm, 1, R, asm.modrm.reg | (asm.rex.r << 3) ) );					
				} else {
					log( "{2.3.1.2}" );
					log( "%s,\t", lib_asm_register( (struct LIB_ASM_STRUCTURE *) &asm, 0, R, asm.modrm.reg | (asm.rex.r << 3) ) );
					log( "[%s]", lib_asm_register( (struct LIB_ASM_STRUCTURE *) &asm, 1, M, asm.modrm.rm | (asm.rex.b << 3) ) );
				}
			} else
			// first operand is a memory access?
			if( asm.instruction.options & M ) {
				log( "{2.3.2}" );
				lib_asm_memory( (struct LIB_ASM_STRUCTURE *) &asm, 0 );
				log( ",\t%s", lib_asm_register( (struct LIB_ASM_STRUCTURE *) &asm, 1, R, asm.modrm.reg | (asm.rex.r << 3) ) );
			}
			// second operand is a memory access?
			else if( asm.instruction.options & (M << 7)) {
				log( "{2.3.3}" );
				log( "%s,\t", lib_asm_register( (struct LIB_ASM_STRUCTURE *) &asm, 0, R, asm.modrm.reg | (asm.rex.r << 3) ) );
				lib_asm_memory( (struct LIB_ASM_STRUCTURE *) &asm, 1 );
			}

			// second operand is a immediete?
			else if( asm.instruction.options & (I << 7) ) {
				log( "{2.3.4}" );
				log( "%s,\t", lib_asm_register( (struct LIB_ASM_STRUCTURE *) &asm, 0, R, asm.modrm.reg | (asm.rex.r << 3) ) );
				lib_asm_immediete( (struct LIB_ASM_STRUCTURE *) &asm );
			}

			else log( "{2.3.5}" );			
		}
	}

	// third operand exist?
	if( (asm.instruction.options >> 14) & 0x7F ) {
		log( "{3}" );
		uint32_t immediete;
		if( asm.instruction.options & (B << 14) ) immediete = *(asm.rip++);
		if( asm.instruction.options & (D << 14) ) { immediete = *((uint32_t *) asm.rip); asm.rip += 4; }

		log( ",\t0x%X", immediete );
	}
