/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	LIB_ASM_REX_base	0x40
#define	LIB_ASM_MODRM_register	0x07	// 0x0111
#define	LIB_ASM_MODRM_MOD_register	0x04

struct LIB_ASM_STRUCTURE_INSTRUCTION {
	char	*mnemonic;
	uint8_t	modrm;
	void	*group;
};

#include	"./asm/data.c"

// output to stdout, and return amount of parsed Bytes 
uint64_t lib_asm( void *rip ) {
	// start parsing from this point
	uint8_t *parse = rip;

	// retrieved Byte
	uint8_t opcode = EMPTY;

	// decoded instruction properties
	struct LIB_ASM_STRUCTURE_INSTRUCTION i = { EMPTY };

	// current register size
	uint8_t size_register = 64;	// 64 bit by default, library doesn't support 32 bit

	// change descriptor to
	uint8_t descriptor = EMPTY;	// default

	// REX
	struct LIB_ASM_STRUCTURE_REX {
		uint8_t w;
		uint8_t r;
		uint8_t x;
		uint8_t b;
	} rex = { EMPTY };

	// The Register Operand Type
	struct LIB_ASM_STRUCTURE_MODRM {
		uint8_t	mod;
		uint8_t	reg;
		uint8_t	rm;
	} modrm = { EMPTY };

	// SIB
	struct LIB_ASM_STRUCTURE_SIB {
		uint8_t scale;
		uint8_t index;
		uint8_t base;
	} sib = { EMPTY };

	// until end of instruction
	while( TRUE ) {
		// obtain opcode
		opcode = *(parse++);

		// ignore opcodes: null or invalid in 64 bit mode
		// http://ref.x86asm.net/coder64.html
		if( opcode == 0x06 || opcode == 0x07 || opcode == 0x0E || opcode == 0x16 || opcode == 0x17 || opcode == 0x1E || opcode == 0x1F || opcode == 0x27 || opcode == 0x26 || opcode == 0x2E || opcode == 0x2F || opcode == 0x36 || opcode == 0x37 || opcode == 0x3E || opcode == 0x3F || opcode == 0x60 || opcode == 0x61 || opcode == 0x62 || opcode == 0x82 || opcode == 0x9A || opcode == 0xC4 || opcode == 0xC5 || opcode == 0xD4 || opcode == 0xD5 || opcode == 0xD6 || opcode == 0xEA ) { log( "invalid or data" ); return (uintptr_t) parse - (uintptr_t) rip; }

		// select different descriptor
		if( opcode == 0x64 ) { descriptor = 'f'; continue; }
		if( opcode == 0x65 ) { descriptor = 'g'; continue; }

		// change size?
		if( opcode == 0x66 ) { size_register = 16; rex.w = TRUE; continue; }	// bit
		if( opcode == 0x67 ) { size_register = 64; continue; }	// bit

		// 0x9B (x87fpu)

		// exclusive memory access?
		if( opcode == 0xF0 ) { log( "lock\t" ); continue; }

		// // REP
		// // if( first == 0xF2 ) { log( "repnz\t" ); continue; }	// or REPNE?
		// // if( first == 0xF3 ) { log( "repe\t" ); continue; }	// or REPZ? | SSE
		// if( first == 0xF2 || first == 0xF3 ) { log( "%2X ", (uint8_t) first ); continue; }

		// REX
		if( (opcode & ~STD_MASK_byte_half) == LIB_ASM_REX_base ) {
			// 64 bit operand size
			rex.w = (opcode >> 3) & TRUE;

			// extension of ModR/M registry field
			rex.r = (opcode >> 2) & TRUE;

			// extension of SIB index
			rex.x = (opcode >> 1) & TRUE;

			// extension of ModR/M r/m or SIB base field
			rex.b = opcode & TRUE;

			// ignore this REX if not last one

			// continue
			continue;
		}

		// done with prefixes
		break;
	}

	// instruction: nop
	if( opcode == 0x90 ) { log( "nop" ); return (uintptr_t) parse - (uintptr_t) rip; }

	// get instruction properties
	i = instruction[ opcode ];

	// 2-Byte opcode?
	if( opcode == 0x0F ) {
		// to do
	}

	// unknown instruction?
	if( ! i.mnemonic ) { log( "unknown" ); return (uintptr_t) parse - (uintptr_t) rip; }
	else log( "%s\t", i.mnemonic );

	// ModR/M exist for this mnemonic?
	if( i.modrm ) {	// yes
		// obtain opcode
		opcode = *(parse++);

		// adressing mode
		modrm.mod = (opcode >> 6);

		// register opcode extension
		modrm.reg = (opcode >> 3) & 7;

		// register memory operand
		modrm.rm = opcode & 7;

		// memory manipulation?
		if( modrm.mod != LIB_ASM_MODRM_MOD_register ) {
			// SIB exist for thie mnemonic?
			if( modrm.rm == 0x04 ) {
				// obtain opcode
				opcode = *(parse++);

				// multipler for index
				sib.scale = 2 ^ (opcode >> 6);

				// register
				sib.index = (opcode >> 3) & 3;

				// register
				sib.base = opcode & 3;
			}
		}


	}

	// amount of parsed Bytes
	return (uintptr_t) parse - (uintptr_t) rip;
}
