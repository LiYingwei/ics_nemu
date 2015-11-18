//
// Created by lyw on 15-10-10.
//
#include <cpu/decode/operand.h>
#include <cpu/reg.h>
#include "cpu/exec/template-start.h"

#define instr call

extern bool CALL_CHANGE_PUSH_LATER;
extern bool DONT_TOUCH_MY_EIP;
static void do_execute () {
#if 1 == DATA_BYTE
    Assert(0, "call DATA_BYTE number bytes is not valid");
#endif
    CALL_CHANGE_PUSH_LATER = true;
    INSTR_PUSH(cpu.eip);
    if(op_src->type == OP_TYPE_IMM) {
        //Log("eip = 0x%x", cpu.eip);
        //Log("MEM($esp = 0x%x) = 0x%x", cpu.esp, MEM_R(cpu.esp));
        cpu.eip += op_src->val;
    }
    else {
        DONT_TOUCH_MY_EIP = true;
        cpu.eip = op_src->val;
    }
    print_asm_template1();
}


make_instr_helper(i)
make_instr_helper(rm)

#include "cpu/exec/template-end.h"
