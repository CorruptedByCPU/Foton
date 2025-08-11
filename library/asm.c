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

	// show instruction
	#ifdef DEBUF
		log( "[0x%2X] %s\t", asm -> opcode_0, asm -> instruction.name );
	#else
		// exception for INSD mnemonic
		if( asm -> opcode_0 == 0x6D && asm -> reg_bits == DWORD ) log( "\033[38;2;255;123;114minsd\033[0m\t" );
		
		// exception for OUTSD mnemonic
		else if( asm -> opcode_0 == 0x6F && asm -> reg_bits == DWORD ) log( "\033[38;2;255;123;114moutsd\033[0m\t" );

		// default
		else log( "\033[38;2;255;123;114m%s\033[0m\t", asm -> instruction.name );
	#endif
	
	// only instruction name?
	if( ! asm -> instruction.options ) goto end;	// yes

	#ifdef DEBUF
		if( asm -> rex_semaphore ) log( "{w%u,r%u,x%u,b%u}", asm -> rex.w, asm -> rex.r, asm -> rex.x, asm -> rex.b );
	#endif

	// - PUSH/POP [0x50-0x5F]
	if( asm -> instruction.options & FR ) {
		#ifdef DEBUF
			log( "{1}" );
		#endif

		// only 64 bit registers
		asm -> reg_bits = QWORD;

		log( "\033[38;2;255;166;87m\0" );

		// show
		log( "%s", lib_asm_register( asm, 0, asm -> opcode_0 & STD_MASK_byte_half | (asm -> rex.b << 3) ) );

		// end
		goto end;
	}

	// - PUSH [0x68, 0x6A]
	if( asm -> instruction.options & I ) {
		#ifdef DEBUF
			log( "{2}" );
		#endif

		log( "\033[38;2;121;192;255m\0" );

		// immediate value
		int32_t value;

		// 1 Byte value
		if( asm -> instruction.options & B ) {
			// retrive 1 Byte
			value = (int8_t) *(asm -> rip++);

			// value signed? (relative)
			if( asm -> instruction.options & FE ) {
				// show absolute address by relative value
				if( value & STD_SIZE_BYTE_sign ) log( "0x%16X", (uintptr_t) asm -> rip + value );
				else log( "0x%16X", asm -> rip + value );
			// no, immediate
			} else log( "%u", *(asm -> rip++) );
		}

		// 4 Byte value
		if( asm -> instruction.options & D ) {
			// retrieve 4 Byte
			value = (int8_t) *(asm -> rip++);

			// value signed? (relative)
			if( asm -> instruction.options & FE ) {
				// show absolute address by relative value
				if( value & STD_SIZE_BYTE_sign ) log( "0x%16X", (uintptr_t) asm -> rip + value );
				else log( "0x%16X", asm -> rip + value );
			// no, immediate
			} else log( "%u", *(asm -> rip++) );
		}

		// end
		goto end;
	}

	// calculate source/destination opcodes
	uint8_t register_rm = asm -> modrm.rm | (asm -> rex.b << 3);
	uint8_t register_reg = asm -> modrm.reg | (asm -> rex.r << 3);

	// ModR/M exist?
	if( asm -> modrm_semaphore ) {
		// direct register addressing mode
		if( asm -> modrm.mod == LIB_ASM_FLAG_MODRM_register ) {
			#ifdef DEBUF
				log( "{3.1}" );
				log( "{mod%u,reg%u,rm%u}", asm -> modrm.mod, asm -> modrm.reg, asm -> modrm.rm );
			#endif

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
			#ifdef DEBUF
				log( "{3.2}" );
				log( "{mod%u,reg%u,rm%u}", asm -> modrm.mod, asm -> modrm.reg, asm -> modrm.rm );
			#endif

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
	} else {
		// second operand is an immediate?
		if( asm -> instruction.options & (I << LIB_ASM_OPTION_FLAG_2nd_operand_shift) ) {
			#ifdef DEBUF
				log( "{4.1}" );
			#endif

			// show
			log( "\033[38;2;255;166;87m%s\033[0m,\t", lib_asm_register( asm, 0, register_reg ) );
			log( "\033[38;2;121;192;255m\0" ); lib_asm_immediate( asm );
		}
	}

	// if there is a third operand (by instruction design), its usually an immediate
	if( asm -> instruction.options & (I << LIB_ASM_OPTION_FLAG_3rd_operand_shift) ) {
		#ifdef DEBUF
			log( "{5}" );
		#endif

		// retrieve immediate according to its size
		if( asm -> instruction.options & (B << LIB_ASM_OPTION_FLAG_3rd_operand_shift) ) log( "\033[0m,\t\033[38;2;121;192;255m0x%2X", *(asm -> rip++) );
		if( asm -> instruction.options & (W << LIB_ASM_OPTION_FLAG_2nd_operand_shift) ) { log( "\033[0m,\t\033[38;2;121;192;255m0x%4X", *((uint16_t *) asm -> rip) ); asm -> rip += 2; }
		if( asm -> instruction.options & (D << LIB_ASM_OPTION_FLAG_3rd_operand_shift) ) { log( "\033[0m,\t\033[38;2;121;192;255m0x%8X", *((uint32_t *) asm -> rip) ); asm -> rip += 4; }
	}

end:
	// amount of parsed Bytes
	return (uintptr_t) asm -> rip - (uintptr_t) rip;
}
