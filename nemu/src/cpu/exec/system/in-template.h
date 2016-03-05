//
// Created by lyw on 16-3-5.
//

#include <cpu/decode/operand.h>
#include <cpu/reg.h>
#include "cpu/exec/template-start.h"
#include "device/port-io.h"
#define instr in

make_helper(concat5(instr,_,i2a,_,SUFFIX))
{
    int len = decode_i_b(eip);
    REG(R_EAX) = pio_read(op_src->val, DATA_BYTE);
    print_asm(str(instr) str(SUFFIX) " $%d, %%eax", op_src->val);
    return len + 1;
}

make_helper(concat5(instr,_,d2a,_,SUFFIX))
{
    REG(R_EAX) = pio_read(op_src->val, DATA_BYTE);
    print_asm(str(instr) str(SUFFIX) " %%edx, %%eax");
    printf("edx = %08x eax = %08x", cpu.edx, cpu.eax);
    return 1;
}

#include "cpu/exec/template-end.h"