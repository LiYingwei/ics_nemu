#include <cpu/decode/operand.h>
#include "cpu/exec/template-start.h"

#define instr bt

static void do_execute () {
    assert(DATA_BYTE == 4);
    if(op_dest->type == OP_TYPE_REG) cpu.CF = (op_dest->val >> op_src->val) & 1;
    else // OP_TYPE_MEM
    {
        uint32_t tmp = swaddr_read(op_dest->addr + 4 * (op_src->val / 32), 4, R_DS);
        cpu.CF = (tmp >> (op_src->val % 32)) & 1;
    }

	print_asm_template2();
}

make_instr_helper(r2rm)

make_helper(concat5(instr,_,i2rm,_,SUFFIX))
{
    assert(DATA_BYTE == 4);
    int len = concat(decode_rm_,SUFFIX)(eip + 1);
    *op_dest = *op_src;
    len += decode_i_b(eip + len + 1);
    if(op_dest->type == OP_TYPE_REG) cpu.CF = (op_dest->val >> op_src->val) & 1;
    else // OP_TYPE_MEM
    {
        uint32_t tmp = swaddr_read(op_dest->addr + 4 * (op_src->val / 32), 4, R_DS);
        cpu.CF = (tmp >> (op_src->val % 32)) & 1;
    }

    print_asm_template2();
    return len + 1;
}
#include "cpu/exec/template-end.h"
