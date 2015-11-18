//
// Created by lyw on 15-10-17.
//
#include <cpu/decode/operand.h>
#include <cpu/reg.h>
#include "cpu/exec/template-start.h"

#define instr push

static void do_execute () {

#if DATA_BYTE == 1
    Assert(op_src->type == OP_TYPE_IMM, "call DATA_BYTE number bytes is not valid");
#endif

    INSTR_PUSH(op_src->val);

    print_asm_template1();
}

make_instr_helper(r);
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(rm);
#endif
make_instr_helper(i);

#include "cpu/exec/template-end.h"
