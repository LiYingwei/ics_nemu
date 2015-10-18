//
// Created by lyw on 15-10-10.
//
#include <cpu/decode/operand.h>
#include <cpu/reg.h>
#include "cpu/exec/template-start.h"

#define instr call

static void do_execute () {
#if 1 == DATA_BYTE
    Assert(0, "call DATA_BYTE number bytes is not valid");
#elif 2 == DATA_BYTE
    cpu.esp -= 2;
    swaddr_write(cpu.esp, 2, cpu.eip);
    cpu.eip += op_src->imm;
#else
    cpu.esp -= 4;
    swaddr_write(cpu.esp, 4, cpu.eip);
    cpu.eip += op_src->imm;
#endif
    print_asm_template1();
}


make_instr_helper(i)

#include "cpu/exec/template-end.h"
