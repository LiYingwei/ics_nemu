//
// Created by lyw on 15-11-14.
//

#include "cpu/exec/helper.h"

#define DATA_BYTE 4

#include "cpu/exec/template-start.h"
extern bool DONT_TOUCH_MY_EIP;
make_helper(ret) {
    cpu.eip = INSTR_POP();
    //Log("MEM($esp = 0x%x) = 0x%x", cpu.esp, MEM_R(cpu.esp));
    DONT_TOUCH_MY_EIP = true;
    print_asm("ret");
    return 1;
}

make_helper(ret_i_w) {
    cpu.eip = INSTR_POP();
    DONT_TOUCH_MY_EIP = true;
    uint16_t imm16 = instr_fetch(eip + 1, 1);
    cpu.esp += imm16;
    print_asm("ret 0x%x", imm16);
    return 2;
}
#include "cpu/exec/template-end.h"

#undef DATA_BYTE
