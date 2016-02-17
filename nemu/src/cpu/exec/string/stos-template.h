//
// Created by lyw on 15-11-15.
//

#include <cpu/decode/operand.h>
#include <cpu/reg.h>
#include "cpu/exec/template-start.h"

#define instr stos
make_helper(concat3(instr,_,SUFFIX)) {
    MEM_W(cpu.edi, cpu.eax);
    if (cpu.DF == 0) cpu.edi += DATA_BYTE;
    else
        cpu.edi -= DATA_BYTE;
    print_asm(str(instr) str(SUFFIX));
    return 1;
}

#include "cpu/exec/template-end.h"
