//
// Created by lyw on 15-11-15.
//


#include <cpu/decode/operand.h>
#include <cpu/reg.h>
#include "cpu/exec/template-start.h"

#define instr cmps
make_helper(concat3(instr,_,SUFFIX)) {
    EFLAGS_ALU(MEM_R(cpu.esi, R_DS), MEM_R(cpu.edi, R_ES), 1, 0);
    DATA_TYPE_S IncDec = 0;
    if(cpu.DF == 0) IncDec = DATA_BYTE; else IncDec = - DATA_BYTE;
    cpu.esi += IncDec;
    cpu.edi += IncDec;

    print_asm("cmps %%es:(%%edi),%%ds:(%%esi)");
    return 1;
}

#include "cpu/exec/template-end.h"
