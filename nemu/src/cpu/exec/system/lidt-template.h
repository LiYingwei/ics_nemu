//
// Created by lyw on 16-2-20.
//
#include <cpu/reg.h>
#include <cpu/decode/operand.h>
#include "cpu/exec/template-start.h"

#define instr lidt

static void do_execute() {
    //Log("%x", op_src->addr);
#if DATA_BYTE == 4
    cpu.IDTR = ((uint64_t)lnaddr_read(op_src->addr, 4))
            + ((uint64_t)lnaddr_read(op_src->addr + 4, 2) << 32);
#else
    cpu.IDTR = ((uint64_t)lnaddr_read(op_src->addr, 2) ) //6 means nothing, because pe=0
            + ((uint64_t)lnaddr_read(op_src->addr + 2, 1) << 16)
            + ((uint64_t)lnaddr_read(op_src->addr + 3, 2) << 24);
#endif
    //print_asm("lidt 0x%012lx", (uint64_t)cpu.GDTR);
    print_asm_template1();
    /*Log("[GDTR]base = %08x, limit = %04x",
        (uint32_t)(cpu.GDTR >> 16), (uint16_t)(cpu.GDTR & 0xFFFF));*/
}

make_instr_helper(rm)

#include "cpu/exec/template-end.h"
