//
// Created by lyw on 15-10-17.
//
#include <cpu/decode/operand.h>
#include <cpu/reg.h>
#include "cpu/exec/template-start.h"

#define instr iret

extern bool DONT_TOUCH_MY_EIP;
static void do_execute () {
    #if DATA_BYTE == 1
    assert(0);
    #endif
    assert(DATA_BYTE == 4);
    if(cpu.protect_enable == 0) {
        cpu.eip = INSTR_POP();
        cpu.cs = INSTR_POP();
        cpu.EFLAGS = INSTR_POP();
    }
    else {
        assert(cpu.VM == 0);Log();
        cpu.eip = INSTR_POP();Log();
        cpu.cs = INSTR_POP();Log();
        cpu.EFLAGS = INSTR_POP();Log();
    }
    DONT_TOUCH_MY_EIP = true;
    print_asm("iret%c", DATA_BYTE == 2 ? ' ':'d');
}

make_helper(concat3(instr,_,SUFFIX))
{
    do_execute();
    return 1;
}

#include "cpu/exec/template-end.h"
