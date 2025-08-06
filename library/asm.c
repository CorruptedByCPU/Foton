/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	LIB_ASM_REX_base	0x40
#define	LIB_ASM_MODRM_register	0x07	// 0x0111

// REX.W = 1	use 64-bit operands (rax, etc.)
// REX.R = 1	extend reg in ModR/M to access r8–r15
// REX.X = 1	extend index in SIB to r8–r15
// REX.B = 1	extend rm or base to r8–r15

// REX
struct LIB_ASM_STRUCTURE_REX {
	uint8_t w;
	uint8_t r;
	uint8_t x;
	uint8_t b;
};

// The Register Operand Type
struct LIB_ASM_STRUCTURE_MODRM {
	uint8_t	mod;
	uint8_t	reg;
	uint8_t	rm;
};

// SIB
struct LIB_ASM_STRUCTURE_SIB {
	uint8_t scale;
	uint8_t index;
	uint8_t base;
};

struct LIB_ASM_STRUCTURE_INSTRUCTION {
	uint8_t		*name;
	uint32_t	options;
	void		*group;
};

struct LIB_ASM_STRUCTURE {
	uint8_t	*rip;
	uint8_t prefix;
	uint8_t opcode_0;
	uint8_t opcode_1;
	uint8_t	rex_semaphore;
	struct LIB_ASM_STRUCTURE_REX			rex;
	uint8_t	modrm_semaphore;
	struct LIB_ASM_STRUCTURE_MODRM			modrm;
	uint32_t displacement;
	uint8_t	sib_semaphore;
	struct LIB_ASM_STRUCTURE_SIB			sib;
	struct LIB_ASM_STRUCTURE_INSTRUCTION	instruction;
	uint8_t	bits;
	uint8_t descriptor;
};

#include	"./asm/data.c"

uint8_t *lib_asm_register( struct LIB_ASM_STRUCTURE *asm, uint8_t operand, uint8_t type, uint8_t reg ) {
	// current bits
	uint8_t bits = asm -> bits;
	

	// select register size by default if not memory type
	if( type & R ) {
		if( asm -> instruction.options >> (7 * operand) & B ) bits = 0;	// 8 bit
		if( asm -> instruction.options >> (7 * operand) & W ) bits = 1;	// 16 bit
		if( asm -> instruction.options >> (7 * operand) & D ) bits = 2;	// 32 bit
		if( asm -> instruction.options >> (7 * operand) & Q ) bits = 3;	// 64 bit
	}
	// by prefix
	if( asm -> prefix == 0x66 ) bits = 1;	// 16 bit
	if( asm -> prefix == 0x67 ) bits = 2;	// 32 bit
	// or by REX (witch have highest priority)
	if( asm -> rex.w ) bits = 3;	// forced 64 bit

	// if there is no 64 bit extension, and we use 8 bit registers
	if( (operand & M) && ! asm -> rex_semaphore && ! bits ) return r_no_rex[ reg ];	// use special array
	
	// otherwise

	// default array
	return r[ bits ][ reg ];
}

void lib_asm_memory( struct LIB_ASM_STRUCTURE *asm, uint8_t operand ) {
	log( "[" );

	if( asm -> modrm.mod == 0x01 ) {
		asm -> displacement = *(asm -> rip++);
	}

	if( asm -> modrm.mod == 0x02 ) {
		asm -> displacement = *((uint32_t *) asm -> rip);
		asm -> rip += 4;
	}

	if( asm -> sib_semaphore ) {
		log( "%s", lib_asm_register( asm, operand, M, asm -> sib.base | (asm -> rex.b << 3) ) );

		if( asm -> sib.index != 0x04 ) {
			log( " + " );
			if( asm -> sib.scale ) log( "(" );
			asm -> bits = 3;
			log( "%s", lib_asm_register( asm, operand, M, asm -> sib.index ) );
			if( asm -> sib.scale ) log( " * %s)", s[ asm -> sib.scale ] );
		}
	} else log( "%s", lib_asm_register( asm, operand, M, asm -> modrm.rm | (asm -> rex.b << 3) ) );

	if( asm -> displacement ) {
		if( asm -> displacement < 256 ) log( " + %u", asm -> displacement );
		else log( " + %u", asm -> displacement );
	}

	log( "]" );
}

void lib_asm_immediete( struct LIB_ASM_STRUCTURE *asm ) {
	int64_t immediete = EMPTY;
	if( asm -> instruction.options & (B << 7) ) {
		immediete = *(asm -> rip++);
		
		log( "0x%2X", (uint8_t) immediete );
	}
	
	if( asm -> instruction.options & (D << 7) ) {
		immediete = *((uint32_t *) asm -> rip);
		asm -> rip += 4;

		log( "0x%8X", (uint32_t) immediete );
	}
}

// output to stdout, and return amount of parsed Bytes 
uint64_t lib_asm( void *rip ) {
	// initialize variables
	struct LIB_ASM_STRUCTURE asm = { EMPTY };
	asm.rip = rip;
	asm.rex_semaphore = asm.rex.w = asm.rex.r = asm.rex.x = asm.rex.b = EMPTY;
	asm.modrm_semaphore = asm.modrm.mod = asm.modrm.reg = asm.modrm.rm = EMPTY;
	asm.sib_semaphore = asm.sib.scale = asm.sib.index = asm.sib.base = EMPTY;
	asm.displacement = EMPTY;
	asm.bits = 3;	// 64 bit default

	// until end of instruction
	while( TRUE ) {
		// obtain opcode
		asm.opcode_0 = *( asm.rip++ );

		// ignore opcodes: null or invalid in 64 bit mode
		// http://ref.x86asm.net/coder64.html
		if( asm.opcode_0 == 0x06 || asm.opcode_0 == 0x07 || asm.opcode_0 == 0x0E || asm.opcode_0 == 0x16 || asm.opcode_0 == 0x17 || asm.opcode_0 == 0x1E || asm.opcode_0 == 0x1F || asm.opcode_0 == 0x27 || asm.opcode_0 == 0x26 || asm.opcode_0 == 0x2E || asm.opcode_0 == 0x2F || asm.opcode_0 == 0x36 || asm.opcode_0 == 0x37 || asm.opcode_0 == 0x3E || asm.opcode_0 == 0x3F || asm.opcode_0 == 0x60 || asm.opcode_0 == 0x61 || asm.opcode_0 == 0x62 || asm.opcode_0 == 0x82 || asm.opcode_0 == 0x9A || asm.opcode_0 == 0xC4 || asm.opcode_0 == 0xC5 || asm.opcode_0 == 0xD4 || asm.opcode_0 == 0xD5 || asm.opcode_0 == 0xD6 || asm.opcode_0 == 0xEA ) { log( "invalid or data" ); return (uintptr_t) asm.rip - (uintptr_t) rip; }

		// select different descriptor
		if( asm.opcode_0 == 0x64 ) { asm.descriptor = 'f'; continue; }
		if( asm.opcode_0 == 0x65 ) { asm.descriptor = 'g'; continue; }

		// change size?
		if( asm.opcode_0 == 0x66 ) { asm.prefix = asm.opcode_0; continue; }	// bit
		if( asm.opcode_0 == 0x67 ) { asm.prefix = asm.opcode_0; continue; }	// bit

		// 0x9B (x87fpu)

		// exclusive memory access?
		if( asm.opcode_0 == 0xF0 ) { log( "lock\t" ); continue; }

		// // REP
		// // if( first == 0xF2 ) { log( "repnz\t" ); continue; }	// or REPNE?
		// // if( first == 0xF3 ) { log( "repe\t" ); continue; }	// or REPZ? | SSE
		// if( first == 0xF2 || first == 0xF3 ) { log( "%2X ", (uint8_t) first ); continue; }

		// REX
		if( (asm.opcode_0 & ~STD_MASK_byte_half) == LIB_ASM_REX_base ) {
			// change behavior of something :)
			asm.rex_semaphore = asm.opcode_0;

			// 64 bit operand size
			asm.rex.w = (asm.opcode_0 >> 3) & TRUE;

			// extension of ModR/M registry field
			asm.rex.r = (asm.opcode_0 >> 2) & TRUE;

			// extension of SIB index
			asm.rex.x = (asm.opcode_0 >> 1) & TRUE;

			// extension of ModR/M r/m or SIB base field
			asm.rex.b = asm.opcode_0 & TRUE;

			// debug
			// log( "REX, w: %b, r: %b, x: %b, b: %b\n", asm.rex.w, asm.rex.r, asm.rex.x, asm.rex.b );

			// ignore this REX if not last one

			// continue
			continue;
		}

		// done with prefixes
		break;
	}

	// instruction: nop
	if( asm.opcode_0 == 0x90 ) { log( "nop" ); return (uintptr_t) asm.rip - (uintptr_t) rip; }

	// get instruction properties
	asm.instruction = i[ asm.opcode_0 ];

	// 2-Byte asm.opcode_0?
	if( asm.opcode_0 == 0x0F ) {
		// to do
	}

	// unknown instruction?
	if( ! asm.instruction.name ) { log( "%2X|unknown", asm.opcode_0 ); return (uintptr_t) asm.rip - (uintptr_t) rip; }

	// ModR/M exist for this mnemonic?
	if( asm.instruction.options & FM ) {	// yes
		// obtain opcode
		asm.modrm_semaphore = *(asm.rip++);

		// adressing mode
		asm.modrm.mod = (asm.modrm_semaphore >> 6);

		// register asm.opcode_0 extension
		asm.modrm.reg = (asm.modrm_semaphore >> 3) & 7;

		// register memory operand
		asm.modrm.rm = asm.modrm_semaphore & 7;

		// debug
		// log( "ModR/M, mod: %2b, reg: %3b, rm: %3b\n", asm.modrm.mod, asm.modrm.reg, asm.modrm.rm );

		// memory manipulation?
		if( asm.modrm.mod != 0x03 ) {
			// SIB exist for this mnemonic?
			if( asm.modrm.rm == 0x04 ) {
				// obtain opcode
				asm.sib_semaphore = *(asm.rip++);

				// multipler for index
				asm.sib.scale = asm.sib_semaphore >> 6;

				// register
				asm.sib.index = (asm.sib_semaphore >> 3) & 7;

				// register
				asm.sib.base = asm.sib_semaphore & 7;

				// debug
				// log( "SIB, scale: %2b, index: %3b, base: %3b\n", asm.sib.scale, asm.sib.index, asm.sib.base );
			}
		}
	}

 	log( "%s\t", asm.instruction.name );

	// register-direct mode?
	if( asm.modrm.mod == 0x03 )	{	// 0b11
		log( "%s,\t%s", lib_asm_register( (struct LIB_ASM_STRUCTURE *) &asm, 0, R, asm.modrm.rm | (asm.rex.b << 3) ), lib_asm_register( (struct LIB_ASM_STRUCTURE *) &asm, 1, R, asm.modrm.reg | (asm.rex.r << 3) ) );
	} else {
		if( asm.instruction.options & FO ) {
			log( "%s", lib_asm_register( (struct LIB_ASM_STRUCTURE *) &asm, 0, R, asm.opcode_0 & STD_MASK_byte_half | (asm.rex.b << 3) ));
		} else {
			if( asm.modrm_semaphore && ! asm.modrm.mod && ! asm.modrm.rm ) {
				if( asm.instruction.options & M ) {
					log( "[%s],\t", lib_asm_register( (struct LIB_ASM_STRUCTURE *) &asm, 0, M, asm.modrm.rm | (asm.rex.b << 3) ) );					
					log( "%s", lib_asm_register( (struct LIB_ASM_STRUCTURE *) &asm, 1, R, asm.modrm.reg | (asm.rex.r << 3) ) );					
				} else {
					log( "%s,\t", lib_asm_register( (struct LIB_ASM_STRUCTURE *) &asm, 0, R, asm.modrm.reg | (asm.rex.r << 3) ) );
					log( "[%s]", lib_asm_register( (struct LIB_ASM_STRUCTURE *) &asm, 1, M, asm.modrm.rm | (asm.rex.b << 3) ) );
				}
			} else
			// first operand is a memory access?
			if( asm.instruction.options & M ) {
				lib_asm_memory( (struct LIB_ASM_STRUCTURE *) &asm, 0 );
				log( ",\t%s", lib_asm_register( (struct LIB_ASM_STRUCTURE *) &asm, 1, R, asm.modrm.reg | (asm.rex.r << 3) ) );
			}
			// second operand is a memory access?
			else if( asm.instruction.options & (M << 7)) {
				log( "%s,\t", lib_asm_register( (struct LIB_ASM_STRUCTURE *) &asm, 0, R, asm.modrm.reg | (asm.rex.r << 3) ) );
				lib_asm_memory( (struct LIB_ASM_STRUCTURE *) &asm, 1 );
			}

			// second operand is a immediete?
			else if( asm.instruction.options & (I << 7) ) {
				log( "%s,\t", lib_asm_register( (struct LIB_ASM_STRUCTURE *) &asm, 0, R, asm.modrm.reg | (asm.rex.r << 3) ) );
				lib_asm_immediete( (struct LIB_ASM_STRUCTURE *) &asm );
			}
		}
	}

	// amount of parsed Bytes
	return (uintptr_t) asm.rip - (uintptr_t) rip;
}
