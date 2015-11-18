//
// Created by lyw on 15-11-15.
//


#include <cpu/decode/operand.h>
#include <cpu/reg.h>
#include "cpu/exec/template-start.h"

#define instr cmps
make_helper(concat3(instr,_,SUFFIX)) {
    EFLAGS_ALU(MEM_R(REG(R_ESI)), MEM_R(REG(R_EDI)), 1, 0);
    DATA_TYPE_S IncDec = 0;
    if(cpu.DF == 0) IncDec = DATA_BYTE; else IncDec = - DATA_BYTE;
    REG(R_ESI) += IncDec;
    REG(R_EDI) += IncDec;
    return 1;
}

#include "cpu/exec/template-end.h"
