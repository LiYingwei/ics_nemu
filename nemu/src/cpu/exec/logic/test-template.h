//
// Created by lyw on 15-10-17.
//
#include <cpu/reg.h>
#include "cpu/exec/template-start.h"

#define instr test

static void do_execute () {
	DATA_TYPE result = op_dest->val & op_src->val;
	int bitcount = 0, x = result & 0xFFFF;
	while(x) {
		if(x&1)bitcount++;
		x >>= 1;
	}
	cpu.EFLAGS.PF = (bitcount + 1) & 1;
	cpu.EFLAGS.CF = 0;
	cpu.EFLAGS.OF = 0;
	cpu.EFLAGS.ZF = (result == 0);
	cpu.EFLAGS.SF = ((result & 0x10000000) != 0);



	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif
make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"
