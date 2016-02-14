//
// Created by lyw on 15-10-17.
//
#include <cpu/reg.h>
#include "cpu/exec/template-start.h"

#define instr test

static void do_execute () {
    DATA_TYPE result = op_dest->val & op_src->val;
    EFLAGS_UPDATE_LOGIC(result);
	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif
make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"
