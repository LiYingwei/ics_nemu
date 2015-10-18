//
// Created by lyw on 15-10-18.
//


#include <cpu/decode/operand.h>
#include <cpu/reg.h>
#include "cpu/exec/template-start.h"

#define instr je

static void do_execute() {
    if(cpu.EFLAGS.ZF) cpu.eip += op_src->imm;
    print_asm_template1();
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"

