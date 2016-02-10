//
// Created by lyw on 15-11-15.
//

#include <cpu/decode/operand.h>
#include <cpu/reg.h>
#include "cpu/exec/template-start.h"

#define instr movs
make_helper(concat3(instr,_,SUFFIX)) {
    MEM_W(REG(R_EDI), MEM_R(REG(R_ESI)));
    DATA_TYPE_S IncDec = 0;
    if(cpu.DF == 0) IncDec = DATA_BYTE; else IncDec = - DATA_BYTE;
    REG(R_ESI) += IncDec;
    REG(R_EDI) += IncDec;
    return 1;
}

#include "cpu/exec/template-end.h"
