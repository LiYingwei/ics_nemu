#ifndef __OPERAND_H__
#define __OPERAND_H__

enum { OP_TYPE_REG, OP_TYPE_MEM, OP_TYPE_IMM };

#define OP_STR_SIZE 40

typedef struct {
	uint32_t type;
	size_t size;
	union {
		uint32_t reg;
<<<<<<< HEAD
		hwaddr_t addr;
=======
		swaddr_t addr;
>>>>>>> 1dd62864fe9b4d0aafd2e0d81070f60c1e34058f
		uint32_t imm;
		int32_t simm;
	};
	uint32_t val;
	char str[OP_STR_SIZE];
} Operand;

typedef struct {
	uint32_t opcode;
	bool is_data_size_16;
	Operand src, dest, src2;
} Operands;

#endif
