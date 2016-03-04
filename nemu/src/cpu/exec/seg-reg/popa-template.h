//
// Created by lyw on 15-10-17.
//
#include <cpu/decode/operand.h>
#include <cpu/reg.h>
#include "cpu/exec/template-start.h"

#define instr popa

static void do_execute () {

#if DATA_BYTE == 1
assert(0);
#endif
    REG(R_EDI) = INSTR_POP();
    REG(R_ESI) = INSTR_POP();
    REG(R_EBP) = INSTR_POP();
    INSTR_POP();
    REG(R_EBX) = INSTR_POP();
    REG(R_EDX) = INSTR_POP();
    REG(R_ECX) = INSTR_POP();
    REG(R_EAX) = INSTR_POP();
    print_asm("popa%c", DATA_BYTE == 2 ? ' ':'d');
}

make_helper(concat3(instr,_,SUFFIX))
{
    do_execute();
    return 1;
}

#include "cpu/exec/template-end.h"
