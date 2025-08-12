/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#define	LIB_ASM_FLAG_REX_base_address		0x40
#define	LIB_ASM_FLAG_MODRM_register		0x03	// 0b11

#define	LIB_ASM_OPTION_FLAG_offset		7
#define	LIB_ASM_OPTION_FLAG_2nd_operand_shift	LIB_ASM_OPTION_FLAG_offset
#define	LIB_ASM_OPTION_FLAG_3rd_operand_shift	(LIB_ASM_OPTION_FLAG_offset + LIB_ASM_OPTION_FLAG_2nd_operand_shift)

// type
#define	R 0x00000001	// 0b00000000000000000000000000000001	// register
#define	M 0x00000002	// 0b00000000000000000000000000000010	// memory
#define	I 0x00000004	// 0b00000000000000000000000000000100	// immediete / offset / relative
// size
#define	B 0x00000008	// 0b00000000000000000000000000001000	// byte
#define	W 0x00000010	// 0b00000000000000000000000000010000	// word
#define	D 0x00000020	// 0b00000000000000000000000000100000	// dword
#define	Q 0x00000040	// 0b00000000000000000000000001000000	// qword
// flag
#define	FE 0x01000000	// 0b00000001000000000000000000000000	// relative address
#define	FD 0x02000000	// 0b00000010000000000000000000000000	// specifies direction of ModR/M > source/destination, if set - RM is destination and REG source
#define	F1 0x04000000	// 0b00000100000000000000000000000000	// strictly definied size of operand 1
#define	F2 0x08000000	// 0b00001000000000000000000000000000	// strictly definied size of operand 2
#define	FR 0x10000000	// 0b00010000000000000000000000000000	// register inside opcode
#define	FS 0x20000000	// 0b00100000000000000000000000000000	// signed
#define	FA 0x40000000	// 0b01000000000000000000000000000000	// accumulator
#define FM 0x80000000	// 0b10000000000000000000000000000000	// modr/m exist

// REX.W = 1	use 64-bit destination operands (rax, etc.)
// REX.R = 1	extend reg in ModR/M to access r8–r15
// REX.X = 1	extend index in SIB to r8–r15
// REX.B = 1	extend rm or base to r8–r15

enum {
	BYTE,
	WORD,
	DWORD,
	QWORD
};

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
	int32_t displacement;
	uint8_t	displacement_size;
	uint8_t col;
	uint8_t	sib_semaphore;
	struct LIB_ASM_STRUCTURE_SIB			sib;
	struct LIB_ASM_STRUCTURE_INSTRUCTION	instruction;
	uint8_t	reg_bits;
	uint8_t mem_bits;
	uint8_t descriptor;
};

void lib_asm_immediate( struct LIB_ASM_STRUCTURE *asm );
void lib_asm_memory( struct LIB_ASM_STRUCTURE *asm );
uint8_t *lib_asm_register( struct LIB_ASM_STRUCTURE *asm, uint8_t operand, uint8_t reg );
