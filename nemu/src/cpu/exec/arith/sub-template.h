#include <cpu/decode/operand.h>
#include <cpu/reg.h>
#include "cpu/exec/template-start.h"

#define instr sub


static void do_execute() {
	DATA_TYPE result = op_dest->val - op_src->val;
	int bitcount = 0, x = result & 0xFFFF;
	while(x) {
		if(x&1)bitcount++;
		x >>= 1;
	}
	OPERAND_W(op_dest, result);

	cpu.EFLAGS.CF = (op_src->val > op_dest->val);
	//cpu.EFLAGS.PF = (bitcount(result & 0xFFFF) + 1) & 1;
	cpu.EFLAGS.PF = (bitcount + 1) & 1;
	cpu.EFLAGS.AF = ((result & 0xFFFF) > (op_dest->val & 0xFFFF));
	cpu.EFLAGS.ZF = (result == 0);
	cpu.EFLAGS.SF = ((result & 0x10000000) != 0);
	cpu.EFLAGS.OF = (result > op_dest->val);

	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif
make_instr_helper(r2rm)
make_instr_helper(rm2r)


#include "cpu/exec/template-end.h"
