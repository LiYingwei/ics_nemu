//
// Created by lyw on 15-10-17.
//
#include <cpu/decode/operand.h>
#include <cpu/reg.h>
#include "cpu/exec/template-start.h"

#define instr pusha

static void do_execute () {

#if DATA_BYTE == 1
assert(0);
#endif
    DATA_TYPE Tmp = REG(R_ESP);
    INSTR_PUSH(REG(R_EAX));
    INSTR_PUSH(REG(R_ECX));
    INSTR_PUSH(REG(R_EDX));
    INSTR_PUSH(REG(R_EBX));
    INSTR_PUSH(Tmp);
    INSTR_PUSH(REG(R_EBP));
    INSTR_PUSH(REG(R_ESI));
    INSTR_PUSH(REG(R_EDI));
    print_asm("pusha%c", DATA_BYTE == 2 ? ' ':'d');
}

make_helper(concat3(instr,_,SUFFIX))
{
    do_execute();
    return 1;
}

#include "cpu/exec/template-end.h"
