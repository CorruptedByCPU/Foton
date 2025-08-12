/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

	//----------------------------------------------------------------------
	// static, structures, definitions
	//----------------------------------------------------------------------
	#include	"./asm/config.h"
	//----------------------------------------------------------------------
	// variables
	//----------------------------------------------------------------------
	#include	"./asm/data.c"
	//----------------------------------------------------------------------
	// routines, procedures
	//----------------------------------------------------------------------
	#include	"./asm/init.c"
	#include	"./asm/immediate.c"
	#include	"./asm/memory.c"
	#include	"./asm/register.c"
	//----------------------------------------------------------------------

// #define DEBUF

uint8_t lib_asm_modrm( struct LIB_ASM_STRUCTURE *asm ) {
	// ModR/M exist?
	if( asm -> modrm_semaphore ) {
		// calculate source/destination opcodes
		uint8_t register_rm = asm -> modrm.rm | (asm -> rex.b << 3);
		uint8_t register_reg = asm -> modrm.reg | (asm -> rex.r << 3);

		// direct register addressing mode
		if( asm -> modrm.mod == LIB_ASM_FLAG_MODRM_register ) {
			// invert source/destination?
			if( asm -> instruction.options & FD ) {
				// thats by instruction design
				register_rm = asm -> modrm.reg | (asm -> rex.r << 3);
				register_reg = asm -> modrm.rm | (asm -> rex.b << 3);
			}

			// show destination first
			log( "\033[38;2;255;166;87m%s\033[0m,\t\033[38;2;255;166;87m%s", lib_asm_register( asm, 0, register_reg ), lib_asm_register( asm, 1, register_rm ) );
		// memory addressing mode
		} else {
			// show destination
			if( asm -> instruction.options & M ) {
				// show
				lib_asm_memory( asm );
				log( "\033[0m,\t\033[38;2;255;166;87m%s", lib_asm_register( asm, 1, register_reg ) );
			// and source
			} else {
				// show
				log( "\033[38;2;255;166;87m%s\033[0m,\t",  lib_asm_register( asm, 0, register_reg ) );
				lib_asm_memory( asm );
			}
		}

		// first and second column filled
		asm -> col += 2;
	// instruction without ModR/M
	} else return FALSE;

	// ModR/M parsed
	return TRUE;
}

void lib_asm_name( struct LIB_ASM_STRUCTURE *asm ) {
	// instruction name
	uint8_t *name = asm -> instruction.name;

	// if doesn't exist
	if( ! name ) {
		// group properties
		struct LIB_ASM_STRUCTURE_INSTRUCTION *group = (struct LIB_ASM_STRUCTURE_INSTRUCTION *) asm -> instruction.group;

		// retrieve name from group
		name = group[ asm -> modrm.reg ].name;
	}

	// show instruction
	#ifdef DEBUF
		// exception for INSD mnemonic
		if( asm -> opcode_0 == 0x6D && asm -> reg_bits == DWORD ) log( "\033[0m[%2X] \033[38;2;255;123;114minsd\t", asm -> opcode_0 );
		
		// exception for OUTSD mnemonic
		else if( asm -> opcode_0 == 0x6F && asm -> reg_bits == DWORD ) log( "\033[0m[%2X] \033[38;2;255;123;114moutsd\t", asm -> opcode_0 );

		// default
		else log( "\033[0m[%2X] \033[38;2;255;123;114m%s\t", asm -> opcode_0, name );
	#else
		// exception for INSD mnemonic
		if( asm -> opcode_0 == 0x6D && asm -> reg_bits == DWORD ) log( "\033[38;2;255;123;114minsd\t" );
		
		// exception for OUTSD mnemonic
		else if( asm -> opcode_0 == 0x6F && asm -> reg_bits == DWORD ) log( "\033[38;2;255;123;114moutsd\t" );

		// default
		else log( "\033[38;2;255;123;114m%s\t", name );
	#endif
}

// output to stdout, and return amount of parsed Bytes 
uint64_t lib_asm( void *rip ) {
	// one-time global environment initialization
	struct LIB_ASM_STRUCTURE asm_variables = { EMPTY }; struct LIB_ASM_STRUCTURE *asm = (struct LIB_ASM_STRUCTURE *) &asm_variables; asm -> rip = rip; uint8_t ready = lib_asm_init( asm );

	// are we ready for interpretation?
	if( ! ready ) {	// nope
		// message
		log( "invalid opcode (0x%2X) or data", asm -> opcode_0 );

		// amount of parsed Bytes
		goto end;
	}

	// instruction name
	lib_asm_name( asm );
	
	// only instruction name?
	if( ! asm -> instruction.options ) goto end;	// yes

	// for instructions outside of any group
	if( ! asm -> instruction.group ) {
		// by default, check instruction with ModR/M existence
		if( ! lib_asm_modrm( asm ) ) {
			// only first operand is a register?
			if( asm -> instruction.options & R ) {
				// show
				log( "\033[38;2;255;166;87m%s", lib_asm_register( asm, 0, asm -> modrm.reg | (asm -> rex.r << 3) ) );
			
				// first operand column filled
				asm -> col++;
			// no, there is no register in any operand
			} else {
				// might be hidden inside opcode (0x50-0x5F)
				if( asm -> instruction.options & FR ) {
					// only 64 bit registers allowed
					asm -> reg_bits = QWORD;

					log( "\033[38;2;255;166;87m\0" );

					// show
					log( "%s", lib_asm_register( asm, 0, asm -> opcode_0 & STD_MASK_byte_half | (asm -> rex.b << 3) ) );

					// end
					goto end;
				}
			}
		}
	// parse group existence
	} else {
		// start memory access
		log( "\033[38;5;202m[" );

		// show register
		log( "\033[38;2;255;166;87m%s", r[ asm -> mem_bits ][ asm -> modrm.rm ] );

		// displacement exist?
		if( asm -> displacement ) log( "\033[0m + \033[38;2;121;192;255m0x" );

		// yes
		if( asm -> displacement > 0xFFFF ) log( "%8X", asm -> displacement );
		else if( asm -> displacement > 0xFF ) log( "%4X", asm -> displacement );
		else if( asm -> displacement ) log( "%2X", asm -> displacement );

		// end memory access
		log( "\033[38;5;202m]" );

		// first operand column filled
		asm -> col++;
	}

	// check for any left operand, which can be immediate
	lib_asm_immediate( asm );

end:
	// amount of parsed Bytes
	return (uintptr_t) asm -> rip - (uintptr_t) rip;
}
