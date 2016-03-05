//
// Created by lyw on 16-3-5.
//

#include <cpu/decode/operand.h>
#include "cpu/exec/template-start.h"
#include "device/port-io.h"
#define instr out

make_helper(concat5(instr,_,i2a,_,SUFFIX))
{
    int len = decode_i_b(eip);
    pio_write(op_src->val, DATA_BYTE, REG(R_EAX));
    return len +1;
}

make_helper(concat5(instr,_,d2a,_,SUFFIX))
{
    pio_write(op_src->val, DATA_BYTE, REG(R_EAX));
    return 1;
}

#include "cpu/exec/template-end.h"