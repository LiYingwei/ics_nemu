//
// Created by lyw on 15-10-21.
//

#include <cpu/decode/operand.h>
#include <cpu/reg.h>
#include "cpu/exec/template-start.h"

#define instr jmp
extern bool DONT_TOUCH_MY_EIP;
static void do_execute() {
    if(op_src->type == OP_TYPE_IMM){
        cpu.eip += (DATA_TYPE_S) (op_src->val);
    }
    else /* op_src->type == OP_TYPE_REG || op_src->type == OP_TYPE_MEM */
    {
        Assert(swaddr_read(cpu.eip, 1) == 0xff, "0xff != indirect");
        DONT_TOUCH_MY_EIP = true;
        cpu.eip = (DATA_TYPE) (op_src->val);
    }
    print_asm_template1();
}

make_instr_helper(si)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(rm)
#endif

#include "cpu/exec/template-end.h"

