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
        cpu.eip += op_src->val;
    }
    else if(op_src->type == OP_TYPE_REG)
    {
        DONT_TOUCH_MY_EIP = true;
        cpu.eip = op_src->val;
    }
    //else panic("Has not impleament yet TAT\n");
    print_asm_template1();
}

make_instr_helper(si)
make_instr_helper(rm)

#include "cpu/exec/template-end.h"

