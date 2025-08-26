/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

// #define DEBUF	// shows additional informations instruction

//=============================================================================
#define	LIB_ASM_COLOR	// colorize

#ifdef LIB_ASM_COLOR
	#define	LIB_ASM_COLOR_DATA		"\033[38;5;154m"
	#define LIB_ASM_COLOR_DEFAULT		"\033[0m"
	#define	LIB_ASM_COLOR_IMMEDIATE		"\033[38;2;121;192;255m"
	#define LIB_ASM_COLOR_INSTRUCTION	"\033[38;2;255;123;114m"
	#define	LIB_ASM_COLOR_MEMORY		"\033[38;5;202m"
	#define	LIB_ASM_COLOR_REGISTER		"\033[38;2;255;166;87m"
	#define	LIB_ASM_COLOR_SCALE			"\033[38;5;208m"
#else
	#define	LIB_ASM_COLOR_DATA
	#define LIB_ASM_COLOR_DEFAULT
	#define	LIB_ASM_COLOR_IMMEDIATE
	#define LIB_ASM_COLOR_INSTRUCTION
	#define	LIB_ASM_COLOR_MEMORY
	#define	LIB_ASM_COLOR_REGISTER
	#define	LIB_ASM_COLOR_SCALE
#endif
//=============================================================================

// separator between operators, by default SPACE
#define	LIB_ASM_SEPARATOR_SIMPLE

#ifdef LIB_ASM_SEPARATOR_SIMPLE
	#define LIB_ASM_SEPARATOR	" "
#else
	#define	LIB_ASM_SEPARATOR	"\t"	// TAB
#endif

// show mnemonic opcode
#define LIB_ASM_OPCODE

#define	LIB_ASM_OPTION_OPERAND_offset	7

// type
#define	M 0x00000001	// memory access
#define	V 0x00000002	// value: immediate, offset, relative
// size
#define	B 0x00000004	// byte
#define	W 0x00000008	// word
#define	D 0x00000010	// dword
// flag
#define FM 0x00200000	// ModR/M exist
#define	FV 0x00400000	// inverted source/destination of ModR/M
#define	F0 0x00800000	// first operand size override
#define	F1 0x01000000	// second operand size override
#define F2 0x02000000	// third operand size override
#define	FD 0x04000000	// register definied by instruction
#define FE 0x08000000	// instruction without register
#define	FR 0x10000000	// relative value
#define	FS 0x20000000	// value is signed
#define	FO 0x40000000	// offset value

enum {
	BYTE,
	WORD,
	DWORD,
	QWORD
};

struct LIB_ASM_STRUCTURE_REX {
	uint8_t semaphore;
	uint8_t w;
	uint8_t r;
	uint8_t x;
	uint8_t b;
};

struct LIB_ASM_STRUCTURE_MODRM {
	uint8_t semaphore;
	uint8_t	mod;
	uint8_t	reg;
	uint8_t	rm;
};

struct LIB_ASM_STRUCTURE_SIB {
	uint8_t semaphore;
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
	uint8_t	*rip;	// next opcode to interpret
	uint8_t opcode;	// current opcode interview
	uint8_t extended;	// 2 Byte opcode?
	uint8_t subset;
	uint8_t	register_bits;	// register size
	uint8_t register_semaphore;	// size changed
	uint8_t memory_bits;	// memory access size
	uint8_t memory_semaphore;	// size changed
	struct LIB_ASM_STRUCTURE_INSTRUCTION	instruction;	// current instruction properties based on opcode
	struct LIB_ASM_STRUCTURE_REX			rex;			// 64 bit registers access (not always available)
	struct LIB_ASM_STRUCTURE_MODRM			modrm;			// extended properties of instruction (not always awailable)
	struct LIB_ASM_STRUCTURE_SIB			sib;			// specification of memory access (not always available)
	int32_t displacement;	// suplementary value for ModR/M|SIB (not always available)
	uint8_t displacement_size;	// BYTE/DWORD
	uint8_t comma_semaphore;	// required for immediate/relative/offset
								// should we put separator before operator
};

// prints immediate/offset/relative value
void lib_asm_value( struct LIB_ASM_STRUCTURE *asm );

// prints instruction name
uint8_t lib_asm_name( struct LIB_ASM_STRUCTURE *asm );

// prints memory access
void lib_asm_memory( struct LIB_ASM_STRUCTURE *asm, uint8_t operand );

// prints register
void lib_asm_register( struct LIB_ASM_STRUCTURE *asm, uint8_t operand, uint8_t reg );
