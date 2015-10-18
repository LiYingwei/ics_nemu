//
// Created by lyw on 15-10-17.
//
#include <cpu/decode/operand.h>
#include <cpu/reg.h>
#include "cpu/exec/template-start.h"

#define instr push

static void do_execute () {
#if DATA_BYTE == 1
    Assert(0, "call DATA_BYTE number bytes is not valid");
#elif DATA_BYTE == 2
    cpu.esp -= 2;
    swaddr_write(cpu.esp, 2, cpu.eip);
#else
    cpu.esp -= 4;
    swaddr_write(cpu.esp, 4, cpu.eip);
#endif
    print_asm_template1();
}

make_instr_helper(r);

#include "cpu/exec/template-end.h"
