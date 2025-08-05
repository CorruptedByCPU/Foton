/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t *r[ 4 ][ 16 ] = {
    { (uint8_t *) "al", (uint8_t *) "cl", (uint8_t *) "dl", (uint8_t *) "bl", (uint8_t *) "spl", (uint8_t *) "bpl", (uint8_t *) "sil", (uint8_t *) "dil", (uint8_t *) "r8b", (uint8_t *) "r9b", (uint8_t *) "r10b", (uint8_t *) "r11b", (uint8_t *) "r12b", (uint8_t *) "r13b", (uint8_t *) "r14b", (uint8_t *) "r15b" },
    { (uint8_t *) "ax", (uint8_t *) "cx", (uint8_t *) "dx", (uint8_t *) "bx", (uint8_t *) "sp", (uint8_t *) "bp", (uint8_t *) "si", (uint8_t *) "di", (uint8_t *) "r8w", (uint8_t *) "r9w", (uint8_t *) "r10w", (uint8_t *) "r11w", (uint8_t *) "r12w", (uint8_t *) "r13w", (uint8_t *) "r14w", (uint8_t *) "r15w" },
    { (uint8_t *) "eax", (uint8_t *) "ecx", (uint8_t *) "edx", (uint8_t *) "ebx", (uint8_t *) "esp", (uint8_t *) "ebp", (uint8_t *) "esi", (uint8_t *) "edi", (uint8_t *) "r8d", (uint8_t *) "r9d", (uint8_t *) "r10d", (uint8_t *) "r11d", (uint8_t *) "r12d", (uint8_t *) "r13d", (uint8_t *) "r14d", (uint8_t *) "r15d" },
    { (uint8_t *) "rax", (uint8_t *) "rcx", (uint8_t *) "rdx", (uint8_t *) "rbx", (uint8_t *) "rsp", (uint8_t *) "rbp", (uint8_t *) "rsi", (uint8_t *) "rdi", (uint8_t *) "r8",  (uint8_t *) "r9",  (uint8_t *) "r10", (uint8_t *) "r11", (uint8_t *) "r12", (uint8_t *) "r13", (uint8_t *) "r14", (uint8_t *) "r15" }
};

uint8_t *r_no_rex[] = {
	(uint8_t *) "al", (uint8_t *) "cl", (uint8_t *) "dl", (uint8_t *) "bl", (uint8_t *) "ah", (uint8_t *) "ch", (uint8_t *) "dh", (uint8_t *) "bh"
};

uint8_t *s[] = { (uint8_t *) "", (uint8_t *) "2", (uint8_t *) "4", (uint8_t *) "8" };

// type
#define	R 0x00000001	// 0b00000000000000000000000000000001	// register
#define	M 0x00000002	// 0b00000000000000000000000000000010	// memory
#define	I 0x00000004	// 0b00000000000000000000000000000100	// immediete / offset / relative
// size
#define	B 0x00000008	// 0b00000000000000000000000000001000	// byte
#define	W 0x00000010	// 0b00000000000000000000000000010000	// word
#define	D 0x00000018	// 0b00000000000000000000000000100000	// dword
#define	Q 0x00000020	// 0b00000000000000000000000001000000	// qword
// flag
#define	FO 0x10000000	// 0b00010000000000000000000000000000	// register inside opcode
#define	FS 0x20000000	// 0b00100000000000000000000000000000	// signed
#define	FA 0x40000000	// 0b01000000000000000000000000000000	// accumulator
#define FM 0x80000000	// 0b10000000000000000000000000000000	// modr/m exist

struct LIB_ASM_STRUCTURE_INSTRUCTION i[] = {
	// 0x00
	{ (uint8_t *) "add", (R|M|B) | (R|B)   << 7 | FM },
	{ (uint8_t *) "add", (R|M)   | (R)     << 7 | FM },
	{ (uint8_t *) "add", (R|B)   | (R|M|B) << 7 | FM },
	{ (uint8_t *) "add", (R)     | (R|M)   << 7 | FM },
	{ (uint8_t *) "add", (R|B)   | (I|B)   << 7 | FM | FA },
	{ (uint8_t *) "add", (R|W)   | (I|B)   << 7 | FM | FA },
	{ EMPTY, EMPTY, EMPTY },
	{ EMPTY, EMPTY, EMPTY },
	// 0x08
	{ (uint8_t *) "or",  (R|M|B) | (R|B)   << 7 | FM },
	{ (uint8_t *) "or",  (R|M)   | (R)     << 7 | FM },
	{ (uint8_t *) "or",  (R|B)   | (R|M|B) << 7 | FM },
	{ (uint8_t *) "or",  (R)     | (R|M)   << 7 | FM },
	{ (uint8_t *) "or",  (R|B)   | (I|B)   << 7 | FM | FA },
	{ (uint8_t *) "or",  (R|W)   | (I|B)   << 7 | FM | FA },
	{ EMPTY, EMPTY, EMPTY },
	{ EMPTY, EMPTY, EMPTY },
	// 0x10
	{ (uint8_t *) "adc", (R|M|B) | (R|B)   << 7 | FM },
	{ (uint8_t *) "adc", (R|M)   | (R)     << 7 | FM },
	{ (uint8_t *) "adc", (R|B)   | (R|M|B) << 7 | FM },
	{ (uint8_t *) "adc", (R)     | (R|M)   << 7 | FM },
	{ (uint8_t *) "adc", (R|B)   | (I|B)   << 7 | FM | FA },
	{ (uint8_t *) "adc", (R|W)   | (I|B)   << 7 | FM | FA },
	{ EMPTY, EMPTY, EMPTY },
	{ EMPTY, EMPTY, EMPTY },
	// 0x18
	{ (uint8_t *) "sbb", (R|M|B) | (R|B)   << 7 | FM },
	{ (uint8_t *) "sbb", (R|M)   | (R)     << 7 | FM },
	{ (uint8_t *) "sbb", (R|B)   | (R|M|B) << 7 | FM },
	{ (uint8_t *) "sbb", (R)     | (R|M)   << 7 | FM },
	{ (uint8_t *) "sbb", (R|B)   | (I|B)   << 7 | FM | FA },
	{ (uint8_t *) "sbb", (R|W)   | (I|B)   << 7 | FM | FA },
	{ EMPTY, EMPTY, EMPTY },
	{ EMPTY, EMPTY, EMPTY },
	// 0x20
	{ (uint8_t *) "and", (R|M|B) | (R|B)   << 7 | FM },
	{ (uint8_t *) "and", (R|M)   | (R)     << 7 | FM },
	{ (uint8_t *) "and", (R|B)   | (R|M|B) << 7 | FM },
	{ (uint8_t *) "and", (R)     | (R|M)   << 7 | FM },
	{ (uint8_t *) "and", (R|B)   | (I|B)   << 7 | FM | FA },
	{ (uint8_t *) "and", (R|W)   | (I|B)   << 7 | FM | FA },
	{ EMPTY, EMPTY, EMPTY },
	{ EMPTY, EMPTY, EMPTY },
	// 0x28
	{ (uint8_t *) "sub", (R|M|B) | (R|B)   << 7 | FM },
	{ (uint8_t *) "sub", (R|M)   | (R)     << 7 | FM },
	{ (uint8_t *) "sub", (R|B)   | (R|M|B) << 7 | FM },
	{ (uint8_t *) "sub", (R)     | (R|M)   << 7 | FM },
	{ (uint8_t *) "sub", (R|B)   | (I|B)   << 7 | FM | FA },
	{ (uint8_t *) "sub", (R|W)   | (I|B)   << 7 | FM | FA },
	{ EMPTY, EMPTY, EMPTY },
	{ EMPTY, EMPTY, EMPTY },
	// 0x30
	{ (uint8_t *) "xor", (R|M|B) | (R|B)   << 7 | FM },
	{ (uint8_t *) "xor", (R|M)   | (R)     << 7 | FM },
	{ (uint8_t *) "xor", (R|B)   | (R|M|B) << 7 | FM },
	{ (uint8_t *) "xor", (R)     | (R|M)   << 7 | FM },
	{ (uint8_t *) "xor", (R|B)   | (I|B)   << 7 | FM | FA },
	{ (uint8_t *) "xor", (R|W)   | (I|B)   << 7 | FM | FA },
	{ EMPTY, EMPTY, EMPTY },
	{ EMPTY, EMPTY, EMPTY },
	// 0x38
	{ (uint8_t *) "cmp", (R|M|B) | (R|B)   << 7 | FM },
	{ (uint8_t *) "cmp", (R|M)   | (R)     << 7 | FM },
	{ (uint8_t *) "cmp", (R|B)   | (R|M|B) << 7 | FM },
	{ (uint8_t *) "cmp", (R)     | (R|M)   << 7 | FM },
	{ (uint8_t *) "cmp", (R|B)   | (I|B)   << 7 | FM | FA },
	{ (uint8_t *) "cmp", (R|W)   | (I|B)   << 7 | FM | FA },
	{ EMPTY, EMPTY, EMPTY },
	{ EMPTY, EMPTY, EMPTY },
	// 0x40
	{ EMPTY, EMPTY, EMPTY },
	{ EMPTY, EMPTY, EMPTY },
	{ EMPTY, EMPTY, EMPTY },
	{ EMPTY, EMPTY, EMPTY },
	{ EMPTY, EMPTY, EMPTY },
	{ EMPTY, EMPTY, EMPTY },
	{ EMPTY, EMPTY, EMPTY },
	{ EMPTY, EMPTY, EMPTY },
	{ EMPTY, EMPTY, EMPTY },
	{ EMPTY, EMPTY, EMPTY },
	{ EMPTY, EMPTY, EMPTY },
	{ EMPTY, EMPTY, EMPTY },
	{ EMPTY, EMPTY, EMPTY },
	{ EMPTY, EMPTY, EMPTY },
	{ EMPTY, EMPTY, EMPTY },
	{ EMPTY, EMPTY, EMPTY },
	// 0x50
	{ (uint8_t *) "push", (R|Q) | FO },
	{ (uint8_t *) "push", (R|Q) | FO },
	{ (uint8_t *) "push", (R|Q) | FO },
	{ (uint8_t *) "push", (R|Q) | FO },
	{ (uint8_t *) "push", (R|Q) | FO },
	{ (uint8_t *) "push", (R|Q) | FO },
	{ (uint8_t *) "push", (R|Q) | FO },
	{ (uint8_t *) "push", (R|Q) | FO },
	// 0x58
	{ (uint8_t *) "pop", (R|Q) | FO },
	{ (uint8_t *) "pop", (R|Q) | FO },
	{ (uint8_t *) "pop", (R|Q) | FO },
	{ (uint8_t *) "pop", (R|Q) | FO },
	{ (uint8_t *) "pop", (R|Q) | FO },
	{ (uint8_t *) "pop", (R|Q) | FO },
	{ (uint8_t *) "pop", (R|Q) | FO },
	{ (uint8_t *) "pop", (R|Q) | FO },
	// // 0x60
	// { EMPTY, EMPTY, EMPTY },
	// { EMPTY, EMPTY, EMPTY },
	// { EMPTY, EMPTY, EMPTY },
	// { (uint8_t *) "movsxd", (R|Q) | (R|M|D) << 7 | FM },	// ? check it further if Q and D can be obsolete
	// { EMPTY, EMPTY, EMPTY },
	// { EMPTY, EMPTY, EMPTY },
	// { EMPTY, EMPTY, EMPTY },
	// { EMPTY, EMPTY, EMPTY }
	// // 0x68

};
