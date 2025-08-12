/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint8_t *r[ 4 ][ 16 ] = {
	{ (uint8_t *) "al", (uint8_t *) "cl", (uint8_t *) "dl", (uint8_t *) "bl", (uint8_t *) "spl", (uint8_t *) "bpl", (uint8_t *) "sil", (uint8_t *) "dil", (uint8_t *) "r8b", (uint8_t *) "r9b", (uint8_t *) "r10b", (uint8_t *) "r11b", (uint8_t *) "r12b", (uint8_t *) "r13b", (uint8_t *) "r14b", (uint8_t *) "r15b" },
	{ (uint8_t *) "ax", (uint8_t *) "cx", (uint8_t *) "dx", (uint8_t *) "bx", (uint8_t *) "sp", (uint8_t *) "bp", (uint8_t *) "si", (uint8_t *) "di", (uint8_t *) "r8w", (uint8_t *) "r9w", (uint8_t *) "r10w", (uint8_t *) "r11w", (uint8_t *) "r12w", (uint8_t *) "r13w", (uint8_t *) "r14w", (uint8_t *) "r15w" },
	{ (uint8_t *) "eax", (uint8_t *) "ecx", (uint8_t *) "edx", (uint8_t *) "ebx", (uint8_t *) "esp", (uint8_t *) "ebp", (uint8_t *) "esi", (uint8_t *) "edi", (uint8_t *) "r8d", (uint8_t *) "r9d", (uint8_t *) "r10d", (uint8_t *) "r11d", (uint8_t *) "r12d", (uint8_t *) "r13d", (uint8_t *) "r14d", (uint8_t *) "r15d" },
	{ (uint8_t *) "rax", (uint8_t *) "rcx", (uint8_t *) "rdx", (uint8_t *) "rbx", (uint8_t *) "rsp", (uint8_t *) "rbp", (uint8_t *) "rsi", (uint8_t *) "rdi", (uint8_t *) "r8", (uint8_t *) "r9", (uint8_t *) "r10", (uint8_t *) "r11", (uint8_t *) "r12", (uint8_t *) "r13", (uint8_t *) "r14", (uint8_t *) "r15" }
};

uint8_t *r_no_rex[] = {
	(uint8_t *) "al", (uint8_t *) "cl", (uint8_t *) "dl", (uint8_t *) "bl", (uint8_t *) "ah", (uint8_t *) "ch", (uint8_t *) "dh", (uint8_t *) "bh"
};

uint8_t *s[] = { (uint8_t *) "", (uint8_t *) "2", (uint8_t *) "4", (uint8_t *) "8" };

struct LIB_ASM_STRUCTURE_INSTRUCTION pause[ 1 ] = {
	{ (uint8_t *) "pause", EMPTY, EMPTY }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION group_1[] = {
	{ (uint8_t *) "add" },
	{ (uint8_t *) "or"  },
	{ (uint8_t *) "adc" },
	{ (uint8_t *) "sbb" },
	{ (uint8_t *) "and" },
	{ (uint8_t *) "sub" },
	{ (uint8_t *) "xor" },
	{ (uint8_t *) "cmp" }
};

struct LIB_ASM_STRUCTURE_INSTRUCTION i[ 256 ] = {
	{ (uint8_t *) "add", (R|M|B) | (R|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | F1 | FD },	// 0x00
	{ (uint8_t *) "add", (R|M|D) | (R|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FD },	// 0x01
	{ (uint8_t *) "add", (R|B) | (R|M|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | F1 },	// 0x02
	{ (uint8_t *) "add", (R|D) | (R|M|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM },	// 0x03
	{ (uint8_t *) "add", (R|B) | (I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | F1 },		// 0x04
	{ (uint8_t *) "add", (R|W) | (I|W) << LIB_ASM_OPTION_FLAG_2nd_operand_shift },		// 0x05
	{ EMPTY, EMPTY, EMPTY },				// 0x06
	{ EMPTY, EMPTY, EMPTY },				// 0x07
	{ (uint8_t *) "or", (R|M|B) | (R|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | F1 | FD },	// 0x08
	{ (uint8_t *) "or", (R|M|D) | (R|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FD },	// 0x09
	{ (uint8_t *) "or", (R|B) | (R|M|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | F1 },	// 0x0A
	{ (uint8_t *) "or", (R|D) | (R|M|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM },	// 0x0B
	{ (uint8_t *) "or", (R|B) | (I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | F1 },		// 0x0C
	{ (uint8_t *) "or", (R|W) | (I|W) << LIB_ASM_OPTION_FLAG_2nd_operand_shift },		// 0x0D
	{ EMPTY, EMPTY, EMPTY },				// 0x0E
	{ EMPTY, EMPTY, EMPTY },				// 0x0F
	{ (uint8_t *) "adc", (R|M|B) | (R|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | F1 | FD },	// 0x10
	{ (uint8_t *) "adc", (R|M|D) | (R|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FD },	// 0x11
	{ (uint8_t *) "adc", (R|B) | (R|M|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | F1 },	// 0x12
	{ (uint8_t *) "adc", (R|D) | (R|M|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM },	// 0x13
	{ (uint8_t *) "adc", (R|B) | (I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | F1 },		// 0x14
	{ (uint8_t *) "adc", (R|W) | (I|W) << LIB_ASM_OPTION_FLAG_2nd_operand_shift },		// 0x15
	{ EMPTY, EMPTY, EMPTY },				// 0x16
	{ EMPTY, EMPTY, EMPTY },				// 0x17
	{ (uint8_t *) "sbb", (R|M|B) | (R|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | F1 | FD },	// 0x18
	{ (uint8_t *) "sbb", (R|M|D) | (R|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FD },	// 0x19
	{ (uint8_t *) "sbb", (R|B) | (R|M|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | F1 },	// 0x1A
	{ (uint8_t *) "sbb", (R|D) | (R|M|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM },	// 0x1B
	{ (uint8_t *) "sbb", (R|B) | (I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | F1 },		// 0x1C
	{ (uint8_t *) "sbb", (R|W) | (I|W) << LIB_ASM_OPTION_FLAG_2nd_operand_shift },		// 0x1D
	{ EMPTY, EMPTY, EMPTY },				// 0x1E
	{ EMPTY, EMPTY, EMPTY },				// 0x1F
	{ (uint8_t *) "and", (R|M|B) | (R|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | F1 | FD },	// 0x20
	{ (uint8_t *) "and", (R|M|D) | (R|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FD },	// 0x21
	{ (uint8_t *) "and", (R|B) | (R|M|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | F1 },	// 0x22
	{ (uint8_t *) "and", (R|D) | (R|M|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM },	// 0x23
	{ (uint8_t *) "and", (R|B) | (I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | F1 },		// 0x24
	{ (uint8_t *) "and", (R|W) | (I|W) << LIB_ASM_OPTION_FLAG_2nd_operand_shift },		// 0x25
	{ EMPTY, EMPTY, EMPTY },				// 0x26
	{ EMPTY, EMPTY, EMPTY },				// 0x27
	{ (uint8_t *) "sub", (R|M|B) | (R|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | F1 | FD },	// 0x28
	{ (uint8_t *) "sub", (R|M|D) | (R|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FD },	// 0x29
	{ (uint8_t *) "sub", (R|B) | (R|M|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | F1 },	// 0x2A
	{ (uint8_t *) "sub", (R|D) | (R|M|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM },	// 0x2B
	{ (uint8_t *) "sub", (R|B) | (I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | F1 },		// 0x2C
	{ (uint8_t *) "sub", (R|W) | (I|W) << LIB_ASM_OPTION_FLAG_2nd_operand_shift },		// 0x2D
	{ EMPTY, EMPTY, EMPTY },				// 0x2E
	{ EMPTY, EMPTY, EMPTY },				// 0x2F
	{ (uint8_t *) "xor", (R|M|B) | (R|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | F1 | FD },	// 0x30
	{ (uint8_t *) "xor", (R|M|D) | (R|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FD },	// 0x31
	{ (uint8_t *) "xor", (R|B) | (R|M|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | F1 },	// 0x32
	{ (uint8_t *) "xor", (R|D) | (R|M|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM },	// 0x33
	{ (uint8_t *) "xor", (R|B) | (I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | F1 },		// 0x34
	{ (uint8_t *) "xor", (R|W) | (I|W) << LIB_ASM_OPTION_FLAG_2nd_operand_shift },		// 0x35
	{ EMPTY, EMPTY, EMPTY },				// 0x36
	{ EMPTY, EMPTY, EMPTY },				// 0x37
	{ (uint8_t *) "cmp", (R|M|B) | (R|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | F1 | FD },	// 0x38
	{ (uint8_t *) "cmp", (R|M|D) | (R|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | FD },	// 0x39
	{ (uint8_t *) "cmp", (R|B) | (R|M|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | F1 },	// 0x3A
	{ (uint8_t *) "cmp", (R|D) | (R|M|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM },	// 0x3B
	{ (uint8_t *) "cmp", (R|B) | (I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | F1 },		// 0x3C
	{ (uint8_t *) "cmp", (R|W) | (I|W) << LIB_ASM_OPTION_FLAG_2nd_operand_shift },		// 0x3D
	{ EMPTY, EMPTY, EMPTY },				// 0x3E
	{ EMPTY, EMPTY, EMPTY },				// 0x3F
	{ EMPTY, EMPTY, EMPTY },				// 0x40
	{ EMPTY, EMPTY, EMPTY },				// 0x41
	{ EMPTY, EMPTY, EMPTY },				// 0x42
	{ EMPTY, EMPTY, EMPTY },				// 0x43
	{ EMPTY, EMPTY, EMPTY },				// 0x44
	{ EMPTY, EMPTY, EMPTY },				// 0x45
	{ EMPTY, EMPTY, EMPTY },				// 0x46
	{ EMPTY, EMPTY, EMPTY },				// 0x47
	{ EMPTY, EMPTY, EMPTY },				// 0x48
	{ EMPTY, EMPTY, EMPTY },				// 0x49
	{ EMPTY, EMPTY, EMPTY },				// 0x4A
	{ EMPTY, EMPTY, EMPTY },				// 0x4B
	{ EMPTY, EMPTY, EMPTY },				// 0x4C
	{ EMPTY, EMPTY, EMPTY },				// 0x4D
	{ EMPTY, EMPTY, EMPTY },				// 0x4E
	{ EMPTY, EMPTY, EMPTY },				// 0x4F
	{ (uint8_t *) "push", FR },			// 0x50
	{ (uint8_t *) "push", FR },			// 0x51
	{ (uint8_t *) "push", FR },			// 0x52
	{ (uint8_t *) "push", FR },			// 0x53
	{ (uint8_t *) "push", FR },			// 0x54
	{ (uint8_t *) "push", FR },			// 0x55
	{ (uint8_t *) "push", FR },			// 0x56
	{ (uint8_t *) "push", FR },			// 0x57
	{ (uint8_t *) "pop", FR },			// 0x58
	{ (uint8_t *) "pop", FR },			// 0x59
	{ (uint8_t *) "pop", FR },			// 0x5A
	{ (uint8_t *) "pop", FR },			// 0x5B
	{ (uint8_t *) "pop", FR },			// 0x5C
	{ (uint8_t *) "pop", FR },			// 0x5D
	{ (uint8_t *) "pop", FR },			// 0x5E
	{ (uint8_t *) "pop", FR },			// 0x5F
	{ EMPTY, EMPTY, EMPTY },				// 0X60
	{ EMPTY, EMPTY, EMPTY },				// 0X61
	{ EMPTY, EMPTY, EMPTY },				// 0X62
	{ (uint8_t *) "movsxd", (R|Q) | (R|M|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | F2 },	// 0x63
	{ EMPTY, EMPTY, EMPTY },				// 0x64
	{ EMPTY, EMPTY, EMPTY },				// 0x65
	{ EMPTY, EMPTY, EMPTY },				// 0x66
	{ EMPTY, EMPTY, EMPTY },				// 0x67
	{ (uint8_t *) "push", (I|D) | F1 },					// 0x68
	{ (uint8_t *) "imul", (R|D) | (R|M|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | (I|D) << LIB_ASM_OPTION_FLAG_3rd_operand_shift | FM, EMPTY },	// 0x69
	{ (uint8_t *) "push", (I|B) | F1 },			// 0x6A
	{ (uint8_t *) "imul", (R|D) | (R|M|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | (I|B) << LIB_ASM_OPTION_FLAG_3rd_operand_shift | FM, EMPTY },	// 0x6B
	{ (uint8_t *) "insb", EMPTY, EMPTY },				// 0x6C
	{ (uint8_t *) "insw", EMPTY, EMPTY },				// 0x6D
	{ (uint8_t *) "outsb", EMPTY, EMPTY },				// 0x6E
	{ (uint8_t *) "outsw", EMPTY, EMPTY },				// 0x6F
	{ (uint8_t *) "jo", (I|B) | FE, EMPTY },				// 0x70
	{ (uint8_t *) "jno", (I|B) | FE, EMPTY },				// 0x71
	{ (uint8_t *) "jb", (I|B) | FE, EMPTY },				// 0x72
	{ (uint8_t *) "jnb", (I|B) | FE, EMPTY },				// 0x73
	{ (uint8_t *) "je", (I|B) | FE, EMPTY },				// 0x74
	{ (uint8_t *) "jne", (I|B) | FE, EMPTY },				// 0x75
	{ (uint8_t *) "jbe", (I|B) | FE, EMPTY },				// 0x76
	{ (uint8_t *) "jnbe", (I|B) | FE, EMPTY },				// 0x77
	{ (uint8_t *) "js", (I|B) | FE, EMPTY },				// 0x78
	{ (uint8_t *) "jns", (I|B) | FE, EMPTY },				// 0x79
	{ (uint8_t *) "jp", (I|B) | FE, EMPTY },				// 0x7A
	{ (uint8_t *) "jnp", (I|B) | FE, EMPTY },				// 0x7B
	{ (uint8_t *) "jl", (I|B) | FE, EMPTY },				// 0x7C
	{ (uint8_t *) "jnl", (I|B) | FE, EMPTY },				// 0x7D
	{ (uint8_t *) "jle", (I|B) | FE, EMPTY },				// 0x7E
	{ (uint8_t *) "jnle", (I|B) | FE, EMPTY },				// 0x7F
	{ EMPTY, (R|M|B) | (I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | F1, group_1 },				// 0x80
	{ EMPTY, (R|M|D) | (I|D) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | F1, group_1 },				// 0x81
	{ EMPTY, EMPTY, EMPTY },				// 0x82
	{ EMPTY, (R|M|D) | (I|B) << LIB_ASM_OPTION_FLAG_2nd_operand_shift | FM | F1, group_1 },				// 0x83
	{ EMPTY, EMPTY, EMPTY },				// 0x84
	{ EMPTY, EMPTY, EMPTY },				// 0x85
	{ EMPTY, EMPTY, EMPTY },				// 0x86
	{ EMPTY, EMPTY, EMPTY },				// 0x87
	{ EMPTY, EMPTY, EMPTY },				// 0x88
	{ EMPTY, EMPTY, EMPTY },				// 0x89
	{ EMPTY, EMPTY, EMPTY },				// 0x8A
	{ EMPTY, EMPTY, EMPTY },				// 0x8B
	{ EMPTY, EMPTY, EMPTY },				// 0x8C
	{ EMPTY, EMPTY, EMPTY },				// 0x8D
	{ EMPTY, EMPTY, EMPTY },				// 0x8E
	{ EMPTY, EMPTY, EMPTY },				// 0x8F
	{ (uint8_t *) "nop", EMPTY, EMPTY },				// 0x90
	{ EMPTY, EMPTY, EMPTY },				// 0x91
	{ EMPTY, EMPTY, EMPTY },				// 0x92
	{ EMPTY, EMPTY, EMPTY },				// 0x93
	{ EMPTY, EMPTY, EMPTY },				// 0x94
	{ EMPTY, EMPTY, EMPTY },				// 0x95
	{ EMPTY, EMPTY, EMPTY },				// 0x96
	{ EMPTY, EMPTY, EMPTY },				// 0x97
	{ EMPTY, EMPTY, EMPTY },				// 0x98
	{ EMPTY, EMPTY, EMPTY },				// 0x99
	{ EMPTY, EMPTY, EMPTY },				// 0x9A
	{ (uint8_t *) "fwait", EMPTY, EMPTY },				// 0x9B
	{ EMPTY, EMPTY, EMPTY },				// 0x9C
	{ EMPTY, EMPTY, EMPTY },				// 0x9D
	{ EMPTY, EMPTY, EMPTY },				// 0x9E
	{ EMPTY, EMPTY, EMPTY },				// 0x9F
	{ EMPTY, EMPTY, EMPTY },				// 0xA0
	{ EMPTY, EMPTY, EMPTY },				// 0xA1
	{ EMPTY, EMPTY, EMPTY },				// 0xA2
	{ EMPTY, EMPTY, EMPTY },				// 0xA3
	{ EMPTY, EMPTY, EMPTY },				// 0xA4
	{ EMPTY, EMPTY, EMPTY },				// 0xA5
	{ EMPTY, EMPTY, EMPTY },				// 0xA6
	{ EMPTY, EMPTY, EMPTY },				// 0xA7
	{ EMPTY, EMPTY, EMPTY },				// 0xA8
	{ EMPTY, EMPTY, EMPTY },				// 0xA9
	{ EMPTY, EMPTY, EMPTY },				// 0xAA
	{ EMPTY, EMPTY, EMPTY },				// 0xAB
	{ EMPTY, EMPTY, EMPTY },				// 0xAC
	{ EMPTY, EMPTY, EMPTY },				// 0xAD
	{ EMPTY, EMPTY, EMPTY },				// 0xAE
	{ EMPTY, EMPTY, EMPTY },				// 0xAF
	{ EMPTY, EMPTY, EMPTY },				// 0xB0
	{ EMPTY, EMPTY, EMPTY },				// 0xB1
	{ EMPTY, EMPTY, EMPTY },				// 0xB2
	{ EMPTY, EMPTY, EMPTY },				// 0xB3
	{ EMPTY, EMPTY, EMPTY },				// 0xB4
	{ EMPTY, EMPTY, EMPTY },				// 0xB5
	{ EMPTY, EMPTY, EMPTY },				// 0xB6
	{ EMPTY, EMPTY, EMPTY },				// 0xB7
	{ EMPTY, EMPTY, EMPTY },				// 0xB8
	{ EMPTY, EMPTY, EMPTY },				// 0xB9
	{ EMPTY, EMPTY, EMPTY },				// 0xBA
	{ EMPTY, EMPTY, EMPTY },				// 0xBB
	{ EMPTY, EMPTY, EMPTY },				// 0xBC
	{ EMPTY, EMPTY, EMPTY },				// 0xBD
	{ EMPTY, EMPTY, EMPTY },				// 0xBE
	{ EMPTY, EMPTY, EMPTY },				// 0xBF
	{ EMPTY, EMPTY, EMPTY },				// 0xC0
	{ EMPTY, EMPTY, EMPTY },				// 0xC1
	{ EMPTY, EMPTY, EMPTY },				// 0xC2
	{ EMPTY, EMPTY, EMPTY },				// 0xC3
	{ EMPTY, EMPTY, EMPTY },				// 0xC4
	{ EMPTY, EMPTY, EMPTY },				// 0xC5
	{ EMPTY, EMPTY, EMPTY },				// 0xC6
	{ EMPTY, EMPTY, EMPTY },				// 0xC7
	{ EMPTY, EMPTY, EMPTY },				// 0xC8
	{ EMPTY, EMPTY, EMPTY },				// 0xC9
	{ EMPTY, EMPTY, EMPTY },				// 0xCA
	{ EMPTY, EMPTY, EMPTY },				// 0xCB
	{ EMPTY, EMPTY, EMPTY },				// 0xCC
	{ EMPTY, EMPTY, EMPTY },				// 0xCD
	{ EMPTY, EMPTY, EMPTY },				// 0xCE
	{ EMPTY, EMPTY, EMPTY },				// 0xCF
	{ EMPTY, EMPTY, EMPTY },				// 0xD0
	{ EMPTY, EMPTY, EMPTY },				// 0xD1
	{ EMPTY, EMPTY, EMPTY },				// 0xD2
	{ EMPTY, EMPTY, EMPTY },				// 0xD3
	{ EMPTY, EMPTY, EMPTY },				// 0xD4
	{ EMPTY, EMPTY, EMPTY },				// 0xD5
	{ EMPTY, EMPTY, EMPTY },				// 0xD6
	{ EMPTY, EMPTY, EMPTY },				// 0xD7
	{ EMPTY, EMPTY, EMPTY },				// 0xD8
	{ EMPTY, EMPTY, EMPTY },				// 0xD9
	{ EMPTY, EMPTY, EMPTY },				// 0xDA
	{ EMPTY, EMPTY, EMPTY },				// 0xDB
	{ EMPTY, EMPTY, EMPTY },				// 0xDC
	{ EMPTY, EMPTY, EMPTY },				// 0xDD
	{ EMPTY, EMPTY, EMPTY },				// 0xDE
	{ EMPTY, EMPTY, EMPTY },				// 0xDF
	{ EMPTY, EMPTY, EMPTY },				// 0xE0
	{ EMPTY, EMPTY, EMPTY },				// 0xE1
	{ EMPTY, EMPTY, EMPTY },				// 0xE2
	{ EMPTY, EMPTY, EMPTY },				// 0xE3
	{ EMPTY, EMPTY, EMPTY },				// 0xE4
	{ EMPTY, EMPTY, EMPTY },				// 0xE5
	{ EMPTY, EMPTY, EMPTY },				// 0xE6
	{ EMPTY, EMPTY, EMPTY },				// 0xE7
	{ EMPTY, EMPTY, EMPTY },				// 0xE8
	{ EMPTY, EMPTY, EMPTY },				// 0xE9
	{ EMPTY, EMPTY, EMPTY },				// 0xEA
	{ EMPTY, EMPTY, EMPTY },				// 0xEB
	{ EMPTY, EMPTY, EMPTY },				// 0xEC
	{ EMPTY, EMPTY, EMPTY },				// 0xED
	{ EMPTY, EMPTY, EMPTY },				// 0xEE
	{ EMPTY, EMPTY, EMPTY },				// 0xEF
	{ EMPTY, EMPTY, EMPTY },				// 0xF0
	{ EMPTY, EMPTY, EMPTY },				// 0xF1
	{ EMPTY, EMPTY, EMPTY },				// 0xF2
	{ EMPTY, EMPTY, EMPTY },				// 0xF3
	{ (uint8_t *) "hlt", EMPTY, EMPTY },				// 0xF4
	{ (uint8_t *) "cmc", EMPTY, EMPTY },				// 0xF5
	{ EMPTY, EMPTY, EMPTY },				// 0xF6
	{ EMPTY, EMPTY, EMPTY },				// 0xF7
	{ (uint8_t *) "clc", EMPTY, EMPTY },				// 0xF8
	{ (uint8_t *) "stc", EMPTY, EMPTY },				// 0xF9
	{ (uint8_t *) "cli", EMPTY, EMPTY },				// 0xFA
	{ (uint8_t *) "sti", EMPTY, EMPTY },				// 0xFB
	{ (uint8_t *) "cld", EMPTY, EMPTY },				// 0xFC
	{ (uint8_t *) "std", EMPTY, EMPTY },				// 0xFD
	{ EMPTY, EMPTY, EMPTY },				// 0xFE
	{ EMPTY, EMPTY, EMPTY }				// 0xFF
};
