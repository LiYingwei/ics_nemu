//
// Created by lyw on 16-2-20.
//
#include <cpu/reg.h>
#include <cpu/decode/operand.h>
#include "cpu/exec/template-start.h"

#define instr lgdt

static void do_execute() {
    //Log("%x", op_src->addr);
#if DATA_BYTE == 4
    cpu.GDTR = ((uint64_t)swaddr_read(op_src->addr, 4, 6))
            + ((uint64_t)swaddr_read(op_src->addr + 4, 2, 6) << 32);
#else
    cpu.GDTR = ((uint64_t)swaddr_read(op_src->addr, 2, 6) ) //6 means nothing, because pe=0
            + ((uint64_t)swaddr_read(op_src->addr + 2, 1, 6) << 16)
            + ((uint64_t)swaddr_read(op_src->addr + 3, 2, 6) << 24);
#endif
    //print_asm("lgdt 0x%012lx", (uint64_t)cpu.GDTR);
    print_asm_template1();
    Log("gdtr = %lx", (uint64_t) cpu.GDTR);
}

make_instr_helper(rm)

#include "cpu/exec/template-end.h"
